#!/usr/bin/env perl

# ====================================================================
# Written by Andy Polyakov <appro@fy.chalmers.se> for the OpenSSL
# project. The module is, however, dual licensed under OpenSSL and
# CRYPTOGAMS licenses depending on where you obtain it. For further
# details see http://www.openssl.org/~appro/cryptogams/.
# ====================================================================

# I let hardware handle unaligned input, except on page boundaries
# (see below for details). Otherwise straightforward implementation
# with X vector in register bank. The module is big-endian [which is
# not big deal as there're no little-endian targets left around].

#			sha256		|	sha512
# 			-m64	-m32	|	-m64	-m32
# --------------------------------------+-----------------------
# PPC970,gcc-4.0.0	+50%	+38%	|	+40%	+410%(*)
# Power6,xlc-7		+150%	+90%	|	+100%	+430%(*)
#
# (*)	64-bit code in 32-bit application context, which actually is
#	on TODO list. It should be noted that for safe deployment in
#	32-bit *mutli-threaded* context asyncronous signals should be
#	blocked upon entry to SHA512 block routine. This is because
#	32-bit signaling procedure invalidates upper halves of GPRs.
#	Context switch procedure preserves them, but not signaling:-(

# Second version is true multi-thread safe. Trouble with the original
# version was that it was using thread local storage pointer register.
# Well, it scrupulously preserved it, but the problem would arise the
# moment asynchronous signal was delivered and signal handler would
# dereference the TLS pointer. While it's never the case in openssl
# application or test suite, we have to respect this scenario and not
# use TLS pointer register. Alternative would be to require caller to
# block signals prior calling this routine. For the record, in 32-bit
# context R2 serves as TLS pointer, while in 64-bit context - R13.

$flavour=shift;
$output =shift;

if ($flavour =~ /64/) {
	$SIZE_T=8;
	$LRSAVE=2*$SIZE_T;
	$STU="stdu";
	$UCMP="cmpld";
	$SHL="sldi";
	$POP="ld";
	$PUSH="std";
} elsif ($flavour =~ /32/) {
	$SIZE_T=4;
	$LRSAVE=$SIZE_T;
	$STU="stwu";
	$UCMP="cmplw";
	$SHL="slwi";
	$POP="lwz";
	$PUSH="stw";
} else { die "nonsense $flavour"; }

$LITTLE_ENDIAN=0;
if ($flavour =~ /le$/) {
	die "little-endian is 64-bit only: $flavour" if ($SIZE_T==4);
	$LITTLE_ENDIAN=1;
}

$0 =~ m/(.*[\/\\])[^\/\\]+$/; $dir=$1;
( $xlate="${dir}ppc-xlate.pl" and -f $xlate ) or
( $xlate="${dir}../../perlasm/ppc-xlate.pl" and -f $xlate) or
die "can't locate ppc-xlate.pl";

open STDOUT,"| $^X $xlate $flavour $output" || die "can't call $xlate: $!";

if ($output =~ /512/) {
	$func="sha512_block_data_order";
	$SZ=8;
	@Sigma0=(28,34,39);
	@Sigma1=(14,18,41);
	@sigma0=(1,  8, 7);
	@sigma1=(19,61, 6);
	$rounds=80;
	$LD="ld";
	$ST="std";
	$ROR="rotrdi";
	$SHR="srdi";
} else {
	$func="sha256_block_data_order";
	$SZ=4;
	@Sigma0=( 2,13,22);
	@Sigma1=( 6,11,25);
	@sigma0=( 7,18, 3);
	@sigma1=(17,19,10);
	$rounds=64;
	$LD="lwz";
	$ST="stw";
	$ROR="rotrwi";
	$SHR="srwi";
}

$FRAME=32*$SIZE_T+16*$SZ;
$LOCALS=6*$SIZE_T;

$sp ="r1";
$toc="r2";
$ctx="r3";	# zapped by $a0
$inp="r4";	# zapped by $a1
$num="r5";	# zapped by $t0

$T  ="r0";
$a0 ="r3";
$a1 ="r4";
$t0 ="r5";
$t1 ="r6";
$Tbl="r7";

$A  ="r8";
$B  ="r9";
$C  ="r10";
$D  ="r11";
$E  ="r12";
$F  ="r13";	$F="r2" if ($SIZE_T==8);# reassigned to exempt TLS pointer
$G  ="r14";
$H  ="r15";

@V=($A,$B,$C,$D,$E,$F,$G,$H);
@X=("r16","r17","r18","r19","r20","r21","r22","r23",
    "r24","r25","r26","r27","r28","r29","r30","r31");

$inp="r31";	# reassigned $inp! aliases with @X[15]

sub ROUND_00_15 {
my ($i,$a,$b,$c,$d,$e,$f,$g,$h)=@_;
$code.=<<___;
	$LD	$T,`$i*$SZ`($Tbl)
	$ROR	$a0,$e,$Sigma1[0]
	$ROR	$a1,$e,$Sigma1[1]
	and	$t0,$f,$e
	andc	$t1,$g,$e
	add	$T,$T,$h
	xor	$a0,$a0,$a1
	$ROR	$a1,$a1,`$Sigma1[2]-$Sigma1[1]`
	or	$t0,$t0,$t1		; Ch(e,f,g)
	add	$T,$T,@X[$i]
	xor	$a0,$a0,$a1		; Sigma1(e)
	add	$T,$T,$t0
	add	$T,$T,$a0

	$ROR	$a0,$a,$Sigma0[0]
	$ROR	$a1,$a,$Sigma0[1]
	and	$t0,$a,$b
	and	$t1,$a,$c
	xor	$a0,$a0,$a1
	$ROR	$a1,$a1,`$Sigma0[2]-$Sigma0[1]`
	xor	$t0,$t0,$t1
	and	$t1,$b,$c
	xor	$a0,$a0,$a1		; Sigma0(a)
	add	$d,$d,$T
	xor	$t0,$t0,$t1		; Maj(a,b,c)
	add	$h,$T,$a0
	add	$h,$h,$t0

___
}

sub ROUND_16_xx {
my ($i,$a,$b,$c,$d,$e,$f,$g,$h)=@_;
$i-=16;
$code.=<<___;
	$ROR	$a0,@X[($i+1)%16],$sigma0[0]
	$ROR	$a1,@X[($i+1)%16],$sigma0[1]
	$ROR	$t0,@X[($i+14)%16],$sigma1[0]
	$ROR	$t1,@X[($i+14)%16],$sigma1[1]
	xor	$a0,$a0,$a1
	$SHR	$a1,@X[($i+1)%16],$sigma0[2]
	xor	$t0,$t0,$t1
	$SHR	$t1,@X[($i+14)%16],$sigma1[2]
	add	@X[$i],@X[$i],@X[($i+9)%16]
	xor	$a0,$a0,$a1		; sigma0(X[(i+1)&0x0f])
	xor	$t0,$t0,$t1		; sigma1(X[(i+14)&0x0f])
	add	@X[$i],@X[$i],$a0
	add	@X[$i],@X[$i],$t0
___
&ROUND_00_15($i,$a,$b,$c,$d,$e,$f,$g,$h);
}

$code=<<___;
.machine	"any"
.text

.globl	$func
.align	6
$func:
	$STU	$sp,-$FRAME($sp)
	mflr	r0
	$SHL	$num,$num,`log(16*$SZ)/log(2)`

	$PUSH	$ctx,`$FRAME-$SIZE_T*22`($sp)

	$PUSH	$toc,`$FRAME-$SIZE_T*20`($sp)
	$PUSH	r13,`$FRAME-$SIZE_T*19`($sp)
	$PUSH	r14,`$FRAME-$SIZE_T*18`($sp)
	$PUSH	r15,`$FRAME-$SIZE_T*17`($sp)
	$PUSH	r16,`$FRAME-$SIZE_T*16`($sp)
	$PUSH	r17,`$FRAME-$SIZE_T*15`($sp)
	$PUSH	r18,`$FRAME-$SIZE_T*14`($sp)
	$PUSH	r19,`$FRAME-$SIZE_T*13`($sp)
	$PUSH	r20,`$FRAME-$SIZE_T*12`($sp)
	$PUSH	r21,`$FRAME-$SIZE_T*11`($sp)
	$PUSH	r22,`$FRAME-$SIZE_T*10`($sp)
	$PUSH	r23,`$FRAME-$SIZE_T*9`($sp)
	$PUSH	r24,`$FRAME-$SIZE_T*8`($sp)
	$PUSH	r25,`$FRAME-$SIZE_T*7`($sp)
	$PUSH	r26,`$FRAME-$SIZE_T*6`($sp)
	$PUSH	r27,`$FRAME-$SIZE_T*5`($sp)
	$PUSH	r28,`$FRAME-$SIZE_T*4`($sp)
	$PUSH	r29,`$FRAME-$SIZE_T*3`($sp)
	$PUSH	r30,`$FRAME-$SIZE_T*2`($sp)
	$PUSH	r31,`$FRAME-$SIZE_T*1`($sp)
	$PUSH	r0,`$FRAME+$LRSAVE`($sp)

	$LD	$A,`0*$SZ`($ctx)
	mr	$inp,r4				; incarnate $inp
	$LD	$B,`1*$SZ`($ctx)
	$LD	$C,`2*$SZ`($ctx)
	$LD	$D,`3*$SZ`($ctx)
	$LD	$E,`4*$SZ`($ctx)
	$LD	$F,`5*$SZ`($ctx)
	$LD	$G,`6*$SZ`($ctx)
	$LD	$H,`7*$SZ`($ctx)

	bl	LPICmeup
LPICedup:
	andi.	r0,$inp,3
	bne	Lunaligned
Laligned:
	add	$num,$inp,$num
	$PUSH	$num,`$FRAME-$SIZE_T*24`($sp)	; end pointer
	$PUSH	$inp,`$FRAME-$SIZE_T*23`($sp)	; inp pointer
	bl	Lsha2_block_private
	b	Ldone

; PowerPC specification allows an implementation to be ill-behaved
; upon unaligned access which crosses page boundary. "Better safe
; than sorry" principle makes me treat it specially. But I don't
; look for particular offending word, but rather for the input
; block which crosses the boundary. Once found that block is aligned
; and hashed separately...
.align	4
Lunaligned:
	subfic	$t1,$inp,4096
	andi.	$t1,$t1,`4096-16*$SZ`	; distance to closest page boundary
	beq	Lcross_page
	$UCMP	$num,$t1
	ble-	Laligned		; didn't cross the page boundary
	subfc	$num,$t1,$num
	add	$t1,$inp,$t1
	$PUSH	$num,`$FRAME-$SIZE_T*25`($sp)	; save real remaining num
	$PUSH	$t1,`$FRAME-$SIZE_T*24`($sp)	; intermediate end pointer
	$PUSH	$inp,`$FRAME-$SIZE_T*23`($sp)	; inp pointer
	bl	Lsha2_block_private
	; $inp equals to the intermediate end pointer here
	$POP	$num,`$FRAME-$SIZE_T*25`($sp)	; restore real remaining num
Lcross_page:
	li	$t1,`16*$SZ/4`
	mtctr	$t1
	addi	r20,$sp,$LOCALS			; aligned spot below the frame
Lmemcpy:
	lbz	r16,0($inp)
	lbz	r17,1($inp)
	lbz	r18,2($inp)
	lbz	r19,3($inp)
	addi	$inp,$inp,4
	stb	r16,0(r20)
	stb	r17,1(r20)
	stb	r18,2(r20)
	stb	r19,3(r20)
	addi	r20,r20,4
	bdnz	Lmemcpy

	$PUSH	$inp,`$FRAME-$SIZE_T*26`($sp)	; save real inp
	addi	$t1,$sp,`$LOCALS+16*$SZ`	; fictitious end pointer
	addi	$inp,$sp,$LOCALS		; fictitious inp pointer
	$PUSH	$num,`$FRAME-$SIZE_T*25`($sp)	; save real num
	$PUSH	$t1,`$FRAME-$SIZE_T*24`($sp)	; end pointer
	$PUSH	$inp,`$FRAME-$SIZE_T*23`($sp)	; inp pointer
	bl	Lsha2_block_private
	$POP	$inp,`$FRAME-$SIZE_T*26`($sp)	; restore real inp
	$POP	$num,`$FRAME-$SIZE_T*25`($sp)	; restore real num
	addic.	$num,$num,`-16*$SZ`		; num--
	bne-	Lunaligned

Ldone:
	$POP	r0,`$FRAME+$LRSAVE`($sp)
	$POP	$toc,`$FRAME-$SIZE_T*20`($sp)
	$POP	r13,`$FRAME-$SIZE_T*19`($sp)
	$POP	r14,`$FRAME-$SIZE_T*18`($sp)
	$POP	r15,`$FRAME-$SIZE_T*17`($sp)
	$POP	r16,`$FRAME-$SIZE_T*16`($sp)
	$POP	r17,`$FRAME-$SIZE_T*15`($sp)
	$POP	r18,`$FRAME-$SIZE_T*14`($sp)
	$POP	r19,`$FRAME-$SIZE_T*13`($sp)
	$POP	r20,`$FRAME-$SIZE_T*12`($sp)
	$POP	r21,`$FRAME-$SIZE_T*11`($sp)
	$POP	r22,`$FRAME-$SIZE_T*10`($sp)
	$POP	r23,`$FRAME-$SIZE_T*9`($sp)
	$POP	r24,`$FRAME-$SIZE_T*8`($sp)
	$POP	r25,`$FRAME-$SIZE_T*7`($sp)
	$POP	r26,`$FRAME-$SIZE_T*6`($sp)
	$POP	r27,`$FRAME-$SIZE_T*5`($sp)
	$POP	r28,`$FRAME-$SIZE_T*4`($sp)
	$POP	r29,`$FRAME-$SIZE_T*3`($sp)
	$POP	r30,`$FRAME-$SIZE_T*2`($sp)
	$POP	r31,`$FRAME-$SIZE_T*1`($sp)
	mtlr	r0
	addi	$sp,$sp,$FRAME
	blr
	.long	0
	.byte	0,12,4,1,0x80,18,3,0
	.long	0

.align	4
Lsha2_block_private:
___
for($i=0;$i<16;$i++) {
$code.=<<___ if ($SZ==4 && !$LITTLE_ENDIAN);
	lwz	@X[$i],`$i*$SZ`($inp)
___
$code.=<<___ if ($SZ==4 && $LITTLE_ENDIAN);
	lwz	$a0,`$i*$SZ`($inp)
	rotlwi	@X[$i],$a0,8
	rlwimi	@X[$i],$a0,24,0,7
	rlwimi	@X[$i],$a0,24,16,23
___
# 64-bit loads are split to 2x32-bit ones, as CPU can't handle
# unaligned 64-bit loads, only 32-bit ones...
$code.=<<___ if ($SZ==8 && !$LITTLE_ENDIAN);
	lwz	$t0,`$i*$SZ`($inp)
	lwz	@X[$i],`$i*$SZ+4`($inp)
	insrdi	@X[$i],$t0,32,0
___
$code.=<<___ if ($SZ==8 && $LITTLE_ENDIAN);
	lwz	$a0,`$i*$SZ`($inp)
	 lwz	$a1,`$i*$SZ+4`($inp)
	rotlwi	$t0,$a0,8
	 rotlwi	@X[$i],$a1,8
	rlwimi	$t0,$a0,24,0,7
	 rlwimi	@X[$i],$a1,24,0,7
	rlwimi	$t0,$a0,24,16,23
	 rlwimi	@X[$i],$a1,24,16,23
	insrdi	@X[$i],$t0,32,0
___
	&ROUND_00_15($i,@V);
	unshift(@V,pop(@V));
}
$code.=<<___;
	li	$T,`$rounds/16-1`
	mtctr	$T
.align	4
Lrounds:
	addi	$Tbl,$Tbl,`16*$SZ`
___
for(;$i<32;$i++) {
	&ROUND_16_xx($i,@V);
	unshift(@V,pop(@V));
}
$code.=<<___;
	bdnz-	Lrounds

	$POP	$ctx,`$FRAME-$SIZE_T*22`($sp)
	$POP	$inp,`$FRAME-$SIZE_T*23`($sp)	; inp pointer
	$POP	$num,`$FRAME-$SIZE_T*24`($sp)	; end pointer
	subi	$Tbl,$Tbl,`($rounds-16)*$SZ`	; rewind Tbl

	$LD	r16,`0*$SZ`($ctx)
	$LD	r17,`1*$SZ`($ctx)
	$LD	r18,`2*$SZ`($ctx)
	$LD	r19,`3*$SZ`($ctx)
	$LD	r20,`4*$SZ`($ctx)
	$LD	r21,`5*$SZ`($ctx)
	$LD	r22,`6*$SZ`($ctx)
	addi	$inp,$inp,`16*$SZ`		; advance inp
	$LD	r23,`7*$SZ`($ctx)
	add	$A,$A,r16
	add	$B,$B,r17
	$PUSH	$inp,`$FRAME-$SIZE_T*23`($sp)
	add	$C,$C,r18
	$ST	$A,`0*$SZ`($ctx)
	add	$D,$D,r19
	$ST	$B,`1*$SZ`($ctx)
	add	$E,$E,r20
	$ST	$C,`2*$SZ`($ctx)
	add	$F,$F,r21
	$ST	$D,`3*$SZ`($ctx)
	add	$G,$G,r22
	$ST	$E,`4*$SZ`($ctx)
	add	$H,$H,r23
	$ST	$F,`5*$SZ`($ctx)
	$ST	$G,`6*$SZ`($ctx)
	$UCMP	$inp,$num
	$ST	$H,`7*$SZ`($ctx)
	bne	Lsha2_block_private
	blr
	.long	0
	.byte	0,12,0x14,0,0,0,0,0
___

# Ugly hack here, because PPC assembler syntax seem to vary too
# much from platforms to platform...
$code.=<<___;
.align	6
LPICmeup:
	mflr	r0
	bcl	20,31,\$+4
	mflr	$Tbl	; vvvvvv "distance" between . and 1st data entry
	addi	$Tbl,$Tbl,`64-8`
	mtlr	r0
	blr
	.long	0
	.byte	0,12,0x14,0,0,0,0,0
	.space	`64-9*4`
___
$code.=<<___ if ($SZ==8);
	.quad	0x428a2f98d728ae22,0x7137449123ef65cd
	.quad	0xb5c0fbcfec4d3b2f,0xe9b5dba58189dbbc
	.quad	0x3956c25bf348b538,0x59f111f1b605d019
	.quad	0x923f82a4af194f9b,0xab1c5ed5da6d8118
	.quad	0xd807aa98a3030242,0x12835b0145706fbe
	.quad	0x243185be4ee4b28c,0x550c7dc3d5ffb4e2
	.quad	0x72be5d74f27b896f,0x80deb1fe3b1696b1
	.quad	0x9bdc06a725c71235,0xc19bf174cf692694
	.quad	0xe49b69c19ef14ad2,0xefbe4786384f25e3
	.quad	0x0fc19dc68b8cd5b5,0x240ca1cc77ac9c65
	.quad	0x2de92c6f592b0275,0x4a7484aa6ea6e483
	.quad	0x5cb0a9dcbd41fbd4,0x76f988da831153b5
	.quad	0x983e5152ee66dfab,0xa831c66d2db43210
	.quad	0xb00327c898fb213f,0xbf597fc7beef0ee4
	.quad	0xc6e00bf33da88fc2,0xd5a79147930aa725
	.quad	0x06ca6351e003826f,0x142929670a0e6e70
	.quad	0x27b70a8546d22ffc,0x2e1b21385c26c926
	.quad	0x4d2c6dfc5ac42aed,0x53380d139d95b3df
	.quad	0x650a73548baf63de,0x766a0abb3c77b2a8
	.quad	0x81c2c92e47edaee6,0x92722c851482353b
	.quad	0xa2bfe8a14cf10364,0xa81a664bbc423001
	.quad	0xc24b8b70d0f89791,0xc76c51a30654be30
	.quad	0xd192e819d6ef5218,0xd69906245565a910
	.quad	0xf40e35855771202a,0x106aa07032bbd1b8
	.quad	0x19a4c116b8d2d0c8,0x1e376c085141ab53
	.quad	0x2748774cdf8eeb99,0x34b0bcb5e19b48a8
	.quad	0x391c0cb3c5c95a63,0x4ed8aa4ae3418acb
	.quad	0x5b9cca4f7763e373,0x682e6ff3d6b2b8a3
	.quad	0x748f82ee5defb2fc,0x78a5636f43172f60
	.quad	0x84c87814a1f0ab72,0x8cc702081a6439ec
	.quad	0x90befffa23631e28,0xa4506cebde82bde9
	.quad	0xbef9a3f7b2c67915,0xc67178f2e372532b
	.quad	0xca273eceea26619c,0xd186b8c721c0c207
	.quad	0xeada7dd6cde0eb1e,0xf57d4f7fee6ed178
	.quad	0x06f067aa72176fba,0x0a637dc5a2c898a6
	.quad	0x113f9804bef90dae,0x1b710b35131c471b
	.quad	0x28db77f523047d84,0x32caab7b40c72493
	.quad	0x3c9ebe0a15c9bebc,0x431d67c49c100d4c
	.quad	0x4cc5d4becb3e42b6,0x597f299cfc657e2a
	.quad	0x5fcb6fab3ad6faec,0x6c44198c4a475817
___
$code.=<<___ if ($SZ==4);
	.long	0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5
	.long	0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5
	.long	0xd807aa98,0x12835b01,0x243185be,0x550c7dc3
	.long	0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174
	.long	0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc
	.long	0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da
	.long	0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7
	.long	0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967
	.long	0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13
	.long	0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85
	.long	0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3
	.long	0xd192e819,0xd6990624,0xf40e3585,0x106aa070
	.long	0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5
	.long	0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3
	.long	0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208
	.long	0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
___

$code =~ s/\`([^\`]*)\`/eval $1/gem;
print $code;
close STDOUT;
