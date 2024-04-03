<?xml version='1.0' encoding='ISO-8859-1' standalone='yes' ?>
<tagfile>
  <compound kind="file">
    <name>apr_anylock.h</name>
    <path>/home/joe/src/apache/apr/apr-util/trunk/include/</path>
    <filename>apr__anylock_8h</filename>
    <class kind="struct">apr_anylock_t</class>
    <class kind="union">apr_anylock_t::apr_anylock_u_t</class>
    <member kind="define">
      <type>#define</type>
      <name>APR_ANYLOCK_LOCK</name>
      <anchorfile>apr__anylock_8h.html</anchorfile>
      <anchor>6a9f237229ff8ba3ec4c0091f8e24709</anchor>
      <arglist>(lck)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_ANYLOCK_TRYLOCK</name>
      <anchorfile>apr__anylock_8h.html</anchorfile>
      <anchor>c78ba03f88f4493daffd496803b3e1e7</anchor>
      <arglist>(lck)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_ANYLOCK_UNLOCK</name>
      <anchorfile>apr__anylock_8h.html</anchorfile>
      <anchor>3616a507b417ccd636df0fb86cc45840</anchor>
      <arglist>(lck)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>apr_base64.h</name>
    <path>/home/joe/src/apache/apr/apr-util/trunk/include/</path>
    <filename>apr__base64_8h</filename>
    <member kind="function">
      <type>int</type>
      <name>apr_base64_encode_len</name>
      <anchorfile>group___a_p_r___util___base64.html</anchorfile>
      <anchor>ga057204c7a32ca4fba300b91664403eb</anchor>
      <arglist>(int len)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>apr_base64_encode</name>
      <anchorfile>group___a_p_r___util___base64.html</anchorfile>
      <anchor>g046c7f12edd1a6bfa229a1f0a979fd78</anchor>
      <arglist>(char *coded_dst, const char *plain_src, int len_plain_src)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>apr_base64_encode_binary</name>
      <anchorfile>group___a_p_r___util___base64.html</anchorfile>
      <anchor>gb3646949fa3248f7c80adacca5750135</anchor>
      <arglist>(char *coded_dst, const unsigned char *plain_src, int len_plain_src)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>apr_base64_decode_len</name>
      <anchorfile>group___a_p_r___util___base64.html</anchorfile>
      <anchor>g14d7c3490149362dbb09bfa1c44c6159</anchor>
      <arglist>(const char *coded_src)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>apr_base64_decode</name>
      <anchorfile>group___a_p_r___util___base64.html</anchorfile>
      <anchor>gf35b1fcaa08f4f291e81b893bf7c5822</anchor>
      <arglist>(char *plain_dst, const char *coded_src)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>apr_base64_decode_binary</name>
      <anchorfile>group___a_p_r___util___base64.html</anchorfile>
      <anchor>g85648b30b638d45618aad6501ef91e40</anchor>
      <arglist>(unsigned char *plain_dst, const char *coded_src)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>apr_buckets.h</name>
    <path>/home/joe/src/apache/apr/apr-util/trunk/include/</path>
    <filename>apr__buckets_8h</filename>
    <class kind="struct">apr_bucket_type_t</class>
    <class kind="struct">apr_bucket</class>
    <class kind="struct">apr_bucket_brigade</class>
    <class kind="struct">apr_bucket_refcount</class>
    <class kind="struct">apr_bucket_heap</class>
    <class kind="struct">apr_bucket_pool</class>
    <class kind="struct">apr_bucket_mmap</class>
    <class kind="struct">apr_bucket_file</class>
    <class kind="union">apr_bucket_structs</class>
    <member kind="define">
      <type>#define</type>
      <name>APR_BUCKET_BUFF_SIZE</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>gf07a6e9e0f1824dba473e006fe235b01</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_BRIGADE_CHECK_CONSISTENCY</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>gcfade45f75165e946670b67625525af8</anchor>
      <arglist>(b)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_BUCKET_CHECK_CONSISTENCY</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g5043988f639cdde471a478ef0ffd50b8</anchor>
      <arglist>(e)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_BRIGADE_SENTINEL</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g55676273b34dc6c92967057a0e1789fb</anchor>
      <arglist>(b)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_BRIGADE_EMPTY</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>gf4bd27296485979250399d436cca1451</anchor>
      <arglist>(b)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_BRIGADE_FIRST</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g1713a11a5c04961ea1fca0d6773b2d6e</anchor>
      <arglist>(b)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_BRIGADE_LAST</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g8aba9ed9096a25d727b01bb63459a281</anchor>
      <arglist>(b)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_BRIGADE_INSERT_HEAD</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g7b2dfa82f2c38e8f40dca044c44a6209</anchor>
      <arglist>(b, e)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_BRIGADE_INSERT_TAIL</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g868dffd635ce2dbd715f9ebcdddd9bbe</anchor>
      <arglist>(b, e)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_BRIGADE_CONCAT</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g49beccfbe23f30cb21f73287b7edb94c</anchor>
      <arglist>(a, b)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_BRIGADE_PREPEND</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g5eb6d0312b4a2d106751383bdaddb8c2</anchor>
      <arglist>(a, b)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_BUCKET_INSERT_BEFORE</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g875b814756c7b894695ee0f4ee48e386</anchor>
      <arglist>(a, b)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_BUCKET_INSERT_AFTER</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g4a69b25ad02c7205dfb31b679993cec2</anchor>
      <arglist>(a, b)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_BUCKET_NEXT</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g52f9fd8b572940839043e56cc5215d24</anchor>
      <arglist>(e)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_BUCKET_PREV</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g5f0b7a368be7db5fe521113966f21db7</anchor>
      <arglist>(e)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_BUCKET_REMOVE</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g474aa518e9d447499978f0f33f638c79</anchor>
      <arglist>(e)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_BUCKET_INIT</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g62807e3c42ebaf0c7ca7b0f5352169b5</anchor>
      <arglist>(e)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_BUCKET_IS_METADATA</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g51de4889d492bdba6482df071e1e7b20</anchor>
      <arglist>(e)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_BUCKET_IS_FLUSH</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g1ce99772926ece2ae0a3c5539a9144db</anchor>
      <arglist>(e)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_BUCKET_IS_EOS</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g873276f0d8129a5b549fd204c57962c6</anchor>
      <arglist>(e)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_BUCKET_IS_FILE</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g8c413557c6e149c668ab756f366902ef</anchor>
      <arglist>(e)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_BUCKET_IS_PIPE</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g75e496497debf6beefb11d68feb33aea</anchor>
      <arglist>(e)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_BUCKET_IS_SOCKET</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>gdb6830a56ac43da909b04bd2435bd84c</anchor>
      <arglist>(e)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_BUCKET_IS_HEAP</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>gff8785fea8b20b12addb46cfec755087</anchor>
      <arglist>(e)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_BUCKET_IS_TRANSIENT</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>ga21bc955981e75df0fde97281d78295a</anchor>
      <arglist>(e)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_BUCKET_IS_IMMORTAL</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>ge1c2e5a41e6bad46caa7f1bcd94c7a3c</anchor>
      <arglist>(e)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_BUCKET_IS_MMAP</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>gcd8f26b880439dbbceda8d6514c108df</anchor>
      <arglist>(e)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_BUCKET_IS_POOL</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>gd403352637f5791429db99cec3a3114c</anchor>
      <arglist>(e)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_BUCKET_ALLOC_SIZE</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g8aa23a6c8ae12eb86fff2b6940992305</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>apr_bucket_destroy</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g08dd3c3b0dfbc4980024b5cbe5ef21c4</anchor>
      <arglist>(e)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>apr_bucket_delete</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g85f29c8023c0c8828ada41718613d2e8</anchor>
      <arglist>(e)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>apr_bucket_read</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g8e5b40655b940454e7c43886d4ead7d9</anchor>
      <arglist>(e, str, len, block)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>apr_bucket_setaside</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g80380462027f1f3d708d1f66a826ede1</anchor>
      <arglist>(e, p)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>apr_bucket_split</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>gc747716a3f3261d5a1ad7d887852fa08</anchor>
      <arglist>(e, point)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>apr_bucket_copy</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>gd2aed8a9000ed36e07dd4d3a42cd85d2</anchor>
      <arglist>(e, c)</arglist>
    </member>
    <member kind="typedef">
      <type>apr_bucket_brigade</type>
      <name>apr_bucket_brigade</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g1e85cecee7a3b35e1713423ad56d72dd</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>apr_bucket</type>
      <name>apr_bucket</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>gab31ae6d5598a2cc199b9a75361912fe</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>apr_bucket_alloc_t</type>
      <name>apr_bucket_alloc_t</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g070fee5f0cf4774a5a708602aeaa00bc</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>apr_bucket_type_t</type>
      <name>apr_bucket_type_t</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>gb671c52af157d10940a4b78aa1df8f90</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>apr_status_t(*)</type>
      <name>apr_brigade_flush</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g0ac2289234d5a275a4641bdf27a8f972</anchor>
      <arglist>(apr_bucket_brigade *bb, void *ctx)</arglist>
    </member>
    <member kind="typedef">
      <type>apr_bucket_refcount</type>
      <name>apr_bucket_refcount</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g83c7c56602c163acdd0d7509f671a78d</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>apr_bucket_heap</type>
      <name>apr_bucket_heap</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g2c684f54f50f41e21b114d14224529f1</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>apr_bucket_pool</type>
      <name>apr_bucket_pool</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>gcef65eca134ea81f6db1addab8a2ca67</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>apr_bucket_mmap</type>
      <name>apr_bucket_mmap</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g152912786f0cdf4e9bd763e1f5235eba</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>apr_bucket_file</type>
      <name>apr_bucket_file</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>gf14ceefc06e4689c23eb7f91e4380e23</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>apr_bucket_structs</type>
      <name>apr_bucket_structs</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g8ba0309f2a7c88e41e6b24e5045bad7c</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <name>apr_read_type_e</name>
      <anchor>g5ef12e1552824398cf3134ebe5fe1ba5</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>APR_BLOCK_READ</name>
      <anchor>gg5ef12e1552824398cf3134ebe5fe1ba59af8ad203f593fa4ae6df7aefc297b67</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>APR_NONBLOCK_READ</name>
      <anchor>gg5ef12e1552824398cf3134ebe5fe1ba53dc18612c7803798dbb446421bef53ba</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>apr_bucket_brigade *</type>
      <name>apr_brigade_create</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>ge6391b41fb52cf572f0ed72392e7cc5f</anchor>
      <arglist>(apr_pool_t *p, apr_bucket_alloc_t *list)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_brigade_destroy</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g8d1d90e229d0e8425e23179e38702a8b</anchor>
      <arglist>(apr_bucket_brigade *b)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_brigade_cleanup</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g47953ffefaf3c3b26847cb384588247c</anchor>
      <arglist>(void *data)</arglist>
    </member>
    <member kind="function">
      <type>apr_bucket_brigade *</type>
      <name>apr_brigade_split</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g6c58253544219af8c89a9d22ffacee26</anchor>
      <arglist>(apr_bucket_brigade *b, apr_bucket *e)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_brigade_partition</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>gcaab4c5fa66602686c4ad1a1ff0da7b6</anchor>
      <arglist>(apr_bucket_brigade *b, apr_off_t point, apr_bucket **after_point)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_brigade_length</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g7592a7a5293e32b8b581ce7d81968bc8</anchor>
      <arglist>(apr_bucket_brigade *bb, int read_all, apr_off_t *length)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_brigade_flatten</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>gb3683e145fafb15220be496e77c9111b</anchor>
      <arglist>(apr_bucket_brigade *bb, char *c, apr_size_t *len)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_brigade_pflatten</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g8cf3d55af2ae924c5637ed2250baa8b4</anchor>
      <arglist>(apr_bucket_brigade *bb, char **c, apr_size_t *len, apr_pool_t *pool)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_brigade_split_line</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g6203eab97431a9aa3777301ec25e357a</anchor>
      <arglist>(apr_bucket_brigade *bbOut, apr_bucket_brigade *bbIn, apr_read_type_e block, apr_off_t maxbytes)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_brigade_to_iovec</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>gebd10c7afc18d5ee105d86c8638158d2</anchor>
      <arglist>(apr_bucket_brigade *b, struct iovec *vec, int *nvec)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_brigade_vputstrs</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g2463542435a4a2ec4524225e05082705</anchor>
      <arglist>(apr_bucket_brigade *b, apr_brigade_flush flush, void *ctx, va_list va)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_brigade_write</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>gd9f6ec4084984506f99c7be3bc3e15e2</anchor>
      <arglist>(apr_bucket_brigade *b, apr_brigade_flush flush, void *ctx, const char *str, apr_size_t nbyte)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_brigade_writev</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g027e7c40c11f857957b8b95b758f75c1</anchor>
      <arglist>(apr_bucket_brigade *b, apr_brigade_flush flush, void *ctx, const struct iovec *vec, apr_size_t nvec)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_brigade_puts</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g30cdc932d2ea64881463de5a013b3eec</anchor>
      <arglist>(apr_bucket_brigade *bb, apr_brigade_flush flush, void *ctx, const char *str)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_brigade_putc</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g6a9fcbc7955169859562ee8614accd9e</anchor>
      <arglist>(apr_bucket_brigade *b, apr_brigade_flush flush, void *ctx, const char c)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_brigade_putstrs</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g05c36115a23e162815cc35f532d9b5e7</anchor>
      <arglist>(apr_bucket_brigade *b, apr_brigade_flush flush, void *ctx,...)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_brigade_printf</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>gb6c616490abe7ecea350dacc085297d3</anchor>
      <arglist>(apr_bucket_brigade *b, apr_brigade_flush flush, void *ctx, const char *fmt,...)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_brigade_vprintf</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g14032492513a841935c5c2e9051ccc65</anchor>
      <arglist>(apr_bucket_brigade *b, apr_brigade_flush flush, void *ctx, const char *fmt, va_list va)</arglist>
    </member>
    <member kind="function">
      <type>apr_bucket *</type>
      <name>apr_brigade_insert_file</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g6a2b211e2f23f623fc4e1a6bb16ca4fe</anchor>
      <arglist>(apr_bucket_brigade *bb, apr_file_t *f, apr_off_t start, apr_off_t len, apr_pool_t *p)</arglist>
    </member>
    <member kind="function">
      <type>apr_bucket_alloc_t *</type>
      <name>apr_bucket_alloc_create</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>ga418b24cfc354dc919a6842154f3fd90</anchor>
      <arglist>(apr_pool_t *p)</arglist>
    </member>
    <member kind="function">
      <type>apr_bucket_alloc_t *</type>
      <name>apr_bucket_alloc_create_ex</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g51d8f03f8d48f20e1a4027585adc4e68</anchor>
      <arglist>(apr_allocator_t *allocator)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>apr_bucket_alloc_destroy</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>gfa8882a7860537f61d91d1737e5c4c9f</anchor>
      <arglist>(apr_bucket_alloc_t *list)</arglist>
    </member>
    <member kind="function">
      <type>void *</type>
      <name>apr_bucket_alloc</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>gb254c96c2e523f5868c09d2ac2c062ac</anchor>
      <arglist>(apr_size_t size, apr_bucket_alloc_t *list)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>apr_bucket_free</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g12432b06513824816d867fefb9711868</anchor>
      <arglist>(void *block)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_bucket_setaside_noop</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g5cd4ee2c0b141e085d6daa25adc3f840</anchor>
      <arglist>(apr_bucket *data, apr_pool_t *pool)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_bucket_setaside_notimpl</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>gca5fc42472c09ccfaa4941818e2cf090</anchor>
      <arglist>(apr_bucket *data, apr_pool_t *pool)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_bucket_split_notimpl</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g7fa00d533df67814feec062050acb5bb</anchor>
      <arglist>(apr_bucket *data, apr_size_t point)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_bucket_copy_notimpl</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>gc6a1492d3906a58e0054b0c8196b28ca</anchor>
      <arglist>(apr_bucket *e, apr_bucket **c)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>apr_bucket_destroy_noop</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>gd46d087cf32369dc3ff1fcfdf68b9d95</anchor>
      <arglist>(void *data)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_bucket_simple_split</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>geef8f4c83b47c73c55b5586cc408e2e8</anchor>
      <arglist>(apr_bucket *b, apr_size_t point)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_bucket_simple_copy</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g9d7a889a0c5cc7d09fa26d395b493189</anchor>
      <arglist>(apr_bucket *a, apr_bucket **b)</arglist>
    </member>
    <member kind="function">
      <type>apr_bucket *</type>
      <name>apr_bucket_shared_make</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g3c2e1107521820ab795a2e213752fee2</anchor>
      <arglist>(apr_bucket *b, void *data, apr_off_t start, apr_size_t length)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>apr_bucket_shared_destroy</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>gbf84b299e8e7476a8132526f35923c40</anchor>
      <arglist>(void *data)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_bucket_shared_split</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g3a92dc8b4af7e33276c5b42d4e803fa8</anchor>
      <arglist>(apr_bucket *b, apr_size_t point)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_bucket_shared_copy</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g14b82b4e383254737967619c4340ff1d</anchor>
      <arglist>(apr_bucket *a, apr_bucket **b)</arglist>
    </member>
    <member kind="function">
      <type>apr_bucket *</type>
      <name>apr_bucket_eos_create</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>gb44d0ad8a3d6e49d1a84da534e692ae9</anchor>
      <arglist>(apr_bucket_alloc_t *list)</arglist>
    </member>
    <member kind="function">
      <type>apr_bucket *</type>
      <name>apr_bucket_eos_make</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>ge9f56bdf98be78a8fb27af76f6613d71</anchor>
      <arglist>(apr_bucket *b)</arglist>
    </member>
    <member kind="function">
      <type>apr_bucket *</type>
      <name>apr_bucket_flush_create</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>gc526b5961d44ba308dc3fde17f399862</anchor>
      <arglist>(apr_bucket_alloc_t *list)</arglist>
    </member>
    <member kind="function">
      <type>apr_bucket *</type>
      <name>apr_bucket_flush_make</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g2ea6dd5951e8c7b131c6f05fbd3a82e4</anchor>
      <arglist>(apr_bucket *b)</arglist>
    </member>
    <member kind="function">
      <type>apr_bucket *</type>
      <name>apr_bucket_immortal_create</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g8a4d5f9cbbaeedee5849715c0fd89524</anchor>
      <arglist>(const char *buf, apr_size_t nbyte, apr_bucket_alloc_t *list)</arglist>
    </member>
    <member kind="function">
      <type>apr_bucket *</type>
      <name>apr_bucket_immortal_make</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g57be0150e55539f5e9953b25f1e8e9b2</anchor>
      <arglist>(apr_bucket *b, const char *buf, apr_size_t nbyte)</arglist>
    </member>
    <member kind="function">
      <type>apr_bucket *</type>
      <name>apr_bucket_transient_create</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g2fb77e1aa8ddb3b1ab307494e18b7533</anchor>
      <arglist>(const char *buf, apr_size_t nbyte, apr_bucket_alloc_t *list)</arglist>
    </member>
    <member kind="function">
      <type>apr_bucket *</type>
      <name>apr_bucket_transient_make</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>gc587487d083659f18ff9e822671d741d</anchor>
      <arglist>(apr_bucket *b, const char *buf, apr_size_t nbyte)</arglist>
    </member>
    <member kind="function">
      <type>apr_bucket *</type>
      <name>apr_bucket_heap_create</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g3900923d436df2b2875f6232b786e033</anchor>
      <arglist>(const char *buf, apr_size_t nbyte, void(*free_func)(void *data), apr_bucket_alloc_t *list)</arglist>
    </member>
    <member kind="function">
      <type>apr_bucket *</type>
      <name>apr_bucket_heap_make</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>gd3904ae2eb5c824f100a7f1cc50f7315</anchor>
      <arglist>(apr_bucket *b, const char *buf, apr_size_t nbyte, void(*free_func)(void *data))</arglist>
    </member>
    <member kind="function">
      <type>apr_bucket *</type>
      <name>apr_bucket_pool_create</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g0dfed65563527d53662430d259336aeb</anchor>
      <arglist>(const char *buf, apr_size_t length, apr_pool_t *pool, apr_bucket_alloc_t *list)</arglist>
    </member>
    <member kind="function">
      <type>apr_bucket *</type>
      <name>apr_bucket_pool_make</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>gb9cdb85120a3b1556a34014cdbf4168e</anchor>
      <arglist>(apr_bucket *b, const char *buf, apr_size_t length, apr_pool_t *pool)</arglist>
    </member>
    <member kind="function">
      <type>apr_bucket *</type>
      <name>apr_bucket_mmap_create</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>ga9ad3b2763b2529aeda752b0758f8267</anchor>
      <arglist>(apr_mmap_t *mm, apr_off_t start, apr_size_t length, apr_bucket_alloc_t *list)</arglist>
    </member>
    <member kind="function">
      <type>apr_bucket *</type>
      <name>apr_bucket_mmap_make</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g86bafecc294063f4ffacacc037d56949</anchor>
      <arglist>(apr_bucket *b, apr_mmap_t *mm, apr_off_t start, apr_size_t length)</arglist>
    </member>
    <member kind="function">
      <type>apr_bucket *</type>
      <name>apr_bucket_socket_create</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g202f596c532528eb55613fa7a934dfa2</anchor>
      <arglist>(apr_socket_t *thissock, apr_bucket_alloc_t *list)</arglist>
    </member>
    <member kind="function">
      <type>apr_bucket *</type>
      <name>apr_bucket_socket_make</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>gdf474ab99206dc6afb2976cf44fa7583</anchor>
      <arglist>(apr_bucket *b, apr_socket_t *thissock)</arglist>
    </member>
    <member kind="function">
      <type>apr_bucket *</type>
      <name>apr_bucket_pipe_create</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g5788c7d7106efcef90cc8eac6e6f7826</anchor>
      <arglist>(apr_file_t *thispipe, apr_bucket_alloc_t *list)</arglist>
    </member>
    <member kind="function">
      <type>apr_bucket *</type>
      <name>apr_bucket_pipe_make</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>gc51915ac9885653a1405c6086f2af800</anchor>
      <arglist>(apr_bucket *b, apr_file_t *thispipe)</arglist>
    </member>
    <member kind="function">
      <type>apr_bucket *</type>
      <name>apr_bucket_file_create</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>ga8fedf2c13927414d34e3b77b46eafc8</anchor>
      <arglist>(apr_file_t *fd, apr_off_t offset, apr_size_t len, apr_pool_t *p, apr_bucket_alloc_t *list)</arglist>
    </member>
    <member kind="function">
      <type>apr_bucket *</type>
      <name>apr_bucket_file_make</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g3303f678b97c86c023a614ab65ad4cd1</anchor>
      <arglist>(apr_bucket *b, apr_file_t *fd, apr_off_t offset, apr_size_t len, apr_pool_t *p)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_bucket_file_enable_mmap</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>gf3caff67d3b6cc6166528ed7b03abb9f</anchor>
      <arglist>(apr_bucket *b, int enabled)</arglist>
    </member>
    <member kind="variable">
      <type>const apr_bucket_type_t</type>
      <name>apr_bucket_type_flush</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g02546431a51c18d5f59b1f2c2eb81fd8</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const apr_bucket_type_t</type>
      <name>apr_bucket_type_eos</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>gcbafc4f9831f24c9bcb8817a5bae8306</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const apr_bucket_type_t</type>
      <name>apr_bucket_type_file</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g062b60a82af96f68e3dd708e6f48fb1d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const apr_bucket_type_t</type>
      <name>apr_bucket_type_heap</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>gd16faf18a40f251d09128e1b58891fbd</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const apr_bucket_type_t</type>
      <name>apr_bucket_type_mmap</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>gf23216833215312bea6acae4cdd86aed</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const apr_bucket_type_t</type>
      <name>apr_bucket_type_pool</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>ga99fdbc30828e8e2168a5c6b929fa353</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const apr_bucket_type_t</type>
      <name>apr_bucket_type_pipe</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g49e70bb0383569e9b170924c3a0ab235</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const apr_bucket_type_t</type>
      <name>apr_bucket_type_immortal</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g8c56a396d94ba5e693f8ddd930c86a25</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const apr_bucket_type_t</type>
      <name>apr_bucket_type_transient</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>gd63b8f60ba9505b747df958951da24b1</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const apr_bucket_type_t</type>
      <name>apr_bucket_type_socket</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g786fd03d89aa92cb3d0317846d7b32c4</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>apr_date.h</name>
    <path>/home/joe/src/apache/apr/apr-util/trunk/include/</path>
    <filename>apr__date_8h</filename>
    <member kind="define">
      <type>#define</type>
      <name>APR_DATE_BAD</name>
      <anchorfile>group___a_p_r___util___date.html</anchorfile>
      <anchor>ga151e18dcfdadd1d137919175ef7bd96</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>apr_date_checkmask</name>
      <anchorfile>group___a_p_r___util___date.html</anchorfile>
      <anchor>g7c7475131bde438838bf57f7863c1678</anchor>
      <arglist>(const char *data, const char *mask)</arglist>
    </member>
    <member kind="function">
      <type>apr_time_t</type>
      <name>apr_date_parse_http</name>
      <anchorfile>group___a_p_r___util___date.html</anchorfile>
      <anchor>g03e62f436ae528733fc0c71c3895dc36</anchor>
      <arglist>(const char *date)</arglist>
    </member>
    <member kind="function">
      <type>apr_time_t</type>
      <name>apr_date_parse_rfc</name>
      <anchorfile>group___a_p_r___util___date.html</anchorfile>
      <anchor>gcef698361f924851a2c6940d9ef5ceca</anchor>
      <arglist>(const char *date)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>apr_dbd.h</name>
    <path>/home/joe/src/apache/apr/apr-util/trunk/include/</path>
    <filename>apr__dbd_8h</filename>
    <member kind="define">
      <type>#define</type>
      <name>APR_DBD_TRANSACTION_COMMIT</name>
      <anchorfile>group___a_p_r___util___d_b_d.html</anchorfile>
      <anchor>g8e5ca5a4c5479a44faf9f2825b827044</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_DBD_TRANSACTION_ROLLBACK</name>
      <anchorfile>group___a_p_r___util___d_b_d.html</anchorfile>
      <anchor>g0734f735259dd95d65a6038b65bcc194</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_DBD_TRANSACTION_IGNORE_ERRORS</name>
      <anchorfile>group___a_p_r___util___d_b_d.html</anchorfile>
      <anchor>g8ddbc7080f2faa7c0510ba4f9fd1aa1f</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>apr_dbd_driver_t</type>
      <name>apr_dbd_driver_t</name>
      <anchorfile>group___a_p_r___util___d_b_d.html</anchorfile>
      <anchor>g46570be872f51bf03a121c6a4d42f343</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>apr_dbd_t</type>
      <name>apr_dbd_t</name>
      <anchorfile>group___a_p_r___util___d_b_d.html</anchorfile>
      <anchor>g865a5e3cf75216b7b59105aa691806a5</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>apr_dbd_transaction_t</type>
      <name>apr_dbd_transaction_t</name>
      <anchorfile>group___a_p_r___util___d_b_d.html</anchorfile>
      <anchor>gb63baa9e8a7c4e2a700a3bdc842a6405</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>apr_dbd_results_t</type>
      <name>apr_dbd_results_t</name>
      <anchorfile>group___a_p_r___util___d_b_d.html</anchorfile>
      <anchor>gb8a533bb665756fa711fbe3ff7fb6d0b</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>apr_dbd_row_t</type>
      <name>apr_dbd_row_t</name>
      <anchorfile>group___a_p_r___util___d_b_d.html</anchorfile>
      <anchor>g9c6eb2d2d432b82dec9fb3e6d98fb376</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>apr_dbd_prepared_t</type>
      <name>apr_dbd_prepared_t</name>
      <anchorfile>group___a_p_r___util___d_b_d.html</anchorfile>
      <anchor>g8aee259b115eb0bd041de73f65a68ce2</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <name>apr_dbd_type_e</name>
      <anchor>gfd17f821a3aaf8194b2866439826e3ca</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>APR_DBD_TYPE_TINY</name>
      <anchor>ggfd17f821a3aaf8194b2866439826e3cabef1e971e17a86470be792fdeedb4c9a</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>APR_DBD_TYPE_UTINY</name>
      <anchor>ggfd17f821a3aaf8194b2866439826e3cafb688b7a74245eecb8dceca55089f41d</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>APR_DBD_TYPE_SHORT</name>
      <anchor>ggfd17f821a3aaf8194b2866439826e3cae0d4a732098074362d6693611d3b4c5a</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>APR_DBD_TYPE_USHORT</name>
      <anchor>ggfd17f821a3aaf8194b2866439826e3ca788a9df38d2a3164b307a511e2a7e695</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>APR_DBD_TYPE_INT</name>
      <anchor>ggfd17f821a3aaf8194b2866439826e3caeb84a5980ca95dba9a849fb3878d6a3b</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>APR_DBD_TYPE_UINT</name>
      <anchor>ggfd17f821a3aaf8194b2866439826e3ca4f742de6028d01556dea20be409a998b</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>APR_DBD_TYPE_LONG</name>
      <anchor>ggfd17f821a3aaf8194b2866439826e3ca829dffc7f8e39d175cbd6289808bec4c</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>APR_DBD_TYPE_ULONG</name>
      <anchor>ggfd17f821a3aaf8194b2866439826e3ca16d7381b12e784d476498ecea1e62556</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>APR_DBD_TYPE_LONGLONG</name>
      <anchor>ggfd17f821a3aaf8194b2866439826e3ca3656373bcf0a066072bb8a4bb24424cc</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>APR_DBD_TYPE_ULONGLONG</name>
      <anchor>ggfd17f821a3aaf8194b2866439826e3ca4e352f61af22456e99e7fb371cc973c1</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>APR_DBD_TYPE_FLOAT</name>
      <anchor>ggfd17f821a3aaf8194b2866439826e3cabc264fdefbf8933b8e87a46cd20d3cf7</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>APR_DBD_TYPE_DOUBLE</name>
      <anchor>ggfd17f821a3aaf8194b2866439826e3cae4eba0afbbb932176213f1031bfa6b15</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>APR_DBD_TYPE_STRING</name>
      <anchor>ggfd17f821a3aaf8194b2866439826e3ca5645d699570b30484f61b4c15eedbc42</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>APR_DBD_TYPE_TEXT</name>
      <anchor>ggfd17f821a3aaf8194b2866439826e3ca22b3086337eff24e821b1d2c8963d537</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>APR_DBD_TYPE_TIME</name>
      <anchor>ggfd17f821a3aaf8194b2866439826e3cae2548b38b38dc1cb37f2f018537f5891</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>APR_DBD_TYPE_DATE</name>
      <anchor>ggfd17f821a3aaf8194b2866439826e3ca9cd5bcbb71647f02ea5ec5abcdf15563</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>APR_DBD_TYPE_DATETIME</name>
      <anchor>ggfd17f821a3aaf8194b2866439826e3ca93f650a5f12155fbe6ed43a60e793b5e</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>APR_DBD_TYPE_TIMESTAMP</name>
      <anchor>ggfd17f821a3aaf8194b2866439826e3ca870a86c5f8d620a4d4a573660a3573ee</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>APR_DBD_TYPE_ZTIMESTAMP</name>
      <anchor>ggfd17f821a3aaf8194b2866439826e3ca69e90e396d90cbfd7012c4afa7fadb4c</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>APR_DBD_TYPE_BLOB</name>
      <anchor>ggfd17f821a3aaf8194b2866439826e3ca9938baebc174dd56b56dcb90da384f08</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>APR_DBD_TYPE_CLOB</name>
      <anchor>ggfd17f821a3aaf8194b2866439826e3ca006f34e981c64a0b462f41d4be240752</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>APR_DBD_TYPE_NULL</name>
      <anchor>ggfd17f821a3aaf8194b2866439826e3ca15af2d239ca828a2c0992f528d45178c</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_dbd_init</name>
      <anchorfile>group___a_p_r___util___d_b_d.html</anchorfile>
      <anchor>g65c75cd4fdce468b4c384874e374423b</anchor>
      <arglist>(apr_pool_t *pool)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_dbd_get_driver</name>
      <anchorfile>group___a_p_r___util___d_b_d.html</anchorfile>
      <anchor>gbf5f24e2c8ff9a78ab4a254b3d6bbf44</anchor>
      <arglist>(apr_pool_t *pool, const char *name, const apr_dbd_driver_t **driver)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_dbd_open</name>
      <anchorfile>group___a_p_r___util___d_b_d.html</anchorfile>
      <anchor>g28f424d11d2e6543daeeec319234bf8c</anchor>
      <arglist>(const apr_dbd_driver_t *driver, apr_pool_t *pool, const char *params, apr_dbd_t **handle)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_dbd_close</name>
      <anchorfile>group___a_p_r___util___d_b_d.html</anchorfile>
      <anchor>g6b615b8a1ea2f15645e6da2e328e3b2b</anchor>
      <arglist>(const apr_dbd_driver_t *driver, apr_dbd_t *handle)</arglist>
    </member>
    <member kind="function">
      <type>const char *</type>
      <name>apr_dbd_name</name>
      <anchorfile>group___a_p_r___util___d_b_d.html</anchorfile>
      <anchor>g31a26096673464b57a28656b151e6b7c</anchor>
      <arglist>(const apr_dbd_driver_t *driver)</arglist>
    </member>
    <member kind="function">
      <type>void *</type>
      <name>apr_dbd_native_handle</name>
      <anchorfile>group___a_p_r___util___d_b_d.html</anchorfile>
      <anchor>gb83a44da154700ee31ca25a38739e0fe</anchor>
      <arglist>(const apr_dbd_driver_t *driver, apr_dbd_t *handle)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>apr_dbd_check_conn</name>
      <anchorfile>group___a_p_r___util___d_b_d.html</anchorfile>
      <anchor>ge7651453a0a89035d44e8aa0fd7b8948</anchor>
      <arglist>(const apr_dbd_driver_t *driver, apr_pool_t *pool, apr_dbd_t *handle)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>apr_dbd_set_dbname</name>
      <anchorfile>group___a_p_r___util___d_b_d.html</anchorfile>
      <anchor>g617861675bf28b0a2a04e66507710ddd</anchor>
      <arglist>(const apr_dbd_driver_t *driver, apr_pool_t *pool, apr_dbd_t *handle, const char *name)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>apr_dbd_transaction_start</name>
      <anchorfile>group___a_p_r___util___d_b_d.html</anchorfile>
      <anchor>g0ed5674f553e53e2a00db77a332e9296</anchor>
      <arglist>(const apr_dbd_driver_t *driver, apr_pool_t *pool, apr_dbd_t *handle, apr_dbd_transaction_t **trans)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>apr_dbd_transaction_end</name>
      <anchorfile>group___a_p_r___util___d_b_d.html</anchorfile>
      <anchor>g76a1a512c08b60b61c573afa39ba784a</anchor>
      <arglist>(const apr_dbd_driver_t *driver, apr_pool_t *pool, apr_dbd_transaction_t *trans)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>apr_dbd_transaction_mode_get</name>
      <anchorfile>group___a_p_r___util___d_b_d.html</anchorfile>
      <anchor>g4b65e6699cab49587d045f60c801f578</anchor>
      <arglist>(const apr_dbd_driver_t *driver, apr_dbd_transaction_t *trans)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>apr_dbd_transaction_mode_set</name>
      <anchorfile>group___a_p_r___util___d_b_d.html</anchorfile>
      <anchor>g30d4bba4826c58161e120aaa350fad19</anchor>
      <arglist>(const apr_dbd_driver_t *driver, apr_dbd_transaction_t *trans, int mode)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>apr_dbd_query</name>
      <anchorfile>group___a_p_r___util___d_b_d.html</anchorfile>
      <anchor>g48b2639510cc7398dc2e00d461934e0d</anchor>
      <arglist>(const apr_dbd_driver_t *driver, apr_dbd_t *handle, int *nrows, const char *statement)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>apr_dbd_select</name>
      <anchorfile>group___a_p_r___util___d_b_d.html</anchorfile>
      <anchor>g69ba5de5c3bc108ff13c4de5d8c879bd</anchor>
      <arglist>(const apr_dbd_driver_t *driver, apr_pool_t *pool, apr_dbd_t *handle, apr_dbd_results_t **res, const char *statement, int random)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>apr_dbd_num_cols</name>
      <anchorfile>group___a_p_r___util___d_b_d.html</anchorfile>
      <anchor>gbd8a416d60cbcd4b8cc4a8509c3039f4</anchor>
      <arglist>(const apr_dbd_driver_t *driver, apr_dbd_results_t *res)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>apr_dbd_num_tuples</name>
      <anchorfile>group___a_p_r___util___d_b_d.html</anchorfile>
      <anchor>g1855a09e38b6324e364579d0e75f767c</anchor>
      <arglist>(const apr_dbd_driver_t *driver, apr_dbd_results_t *res)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>apr_dbd_get_row</name>
      <anchorfile>group___a_p_r___util___d_b_d.html</anchorfile>
      <anchor>gff7f18d3427ab85e6f740becef0d7ad5</anchor>
      <arglist>(const apr_dbd_driver_t *driver, apr_pool_t *pool, apr_dbd_results_t *res, apr_dbd_row_t **row, int rownum)</arglist>
    </member>
    <member kind="function">
      <type>const char *</type>
      <name>apr_dbd_get_entry</name>
      <anchorfile>group___a_p_r___util___d_b_d.html</anchorfile>
      <anchor>g41ad06f5c9436451589aaa0cbbb0cad8</anchor>
      <arglist>(const apr_dbd_driver_t *driver, apr_dbd_row_t *row, int col)</arglist>
    </member>
    <member kind="function">
      <type>const char *</type>
      <name>apr_dbd_get_name</name>
      <anchorfile>group___a_p_r___util___d_b_d.html</anchorfile>
      <anchor>g3d86bd9523f4db1b87418e1f310bdf3b</anchor>
      <arglist>(const apr_dbd_driver_t *driver, apr_dbd_results_t *res, int col)</arglist>
    </member>
    <member kind="function">
      <type>const char *</type>
      <name>apr_dbd_error</name>
      <anchorfile>group___a_p_r___util___d_b_d.html</anchorfile>
      <anchor>g45831b321056c86a7eae233b2d2bbeb7</anchor>
      <arglist>(const apr_dbd_driver_t *driver, apr_dbd_t *handle, int errnum)</arglist>
    </member>
    <member kind="function">
      <type>const char *</type>
      <name>apr_dbd_escape</name>
      <anchorfile>group___a_p_r___util___d_b_d.html</anchorfile>
      <anchor>g463bd686a0b381da2966b72eccf45a0c</anchor>
      <arglist>(const apr_dbd_driver_t *driver, apr_pool_t *pool, const char *string, apr_dbd_t *handle)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>apr_dbd_prepare</name>
      <anchorfile>group___a_p_r___util___d_b_d.html</anchorfile>
      <anchor>ga19b6f9e8f3842b1b08f11916111cd99</anchor>
      <arglist>(const apr_dbd_driver_t *driver, apr_pool_t *pool, apr_dbd_t *handle, const char *query, const char *label, apr_dbd_prepared_t **statement)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>apr_dbd_pquery</name>
      <anchorfile>group___a_p_r___util___d_b_d.html</anchorfile>
      <anchor>g56138ae630391b1a013bd43358cdcf4b</anchor>
      <arglist>(const apr_dbd_driver_t *driver, apr_pool_t *pool, apr_dbd_t *handle, int *nrows, apr_dbd_prepared_t *statement, int nargs, const char **args)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>apr_dbd_pselect</name>
      <anchorfile>group___a_p_r___util___d_b_d.html</anchorfile>
      <anchor>g9244fc2997c87ef89358f46ab7a1abc0</anchor>
      <arglist>(const apr_dbd_driver_t *driver, apr_pool_t *pool, apr_dbd_t *handle, apr_dbd_results_t **res, apr_dbd_prepared_t *statement, int random, int nargs, const char **args)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>apr_dbd_pvquery</name>
      <anchorfile>group___a_p_r___util___d_b_d.html</anchorfile>
      <anchor>g95c0ac1458a94fa0161d82bb4dbe738d</anchor>
      <arglist>(const apr_dbd_driver_t *driver, apr_pool_t *pool, apr_dbd_t *handle, int *nrows, apr_dbd_prepared_t *statement,...)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>apr_dbd_pvselect</name>
      <anchorfile>group___a_p_r___util___d_b_d.html</anchorfile>
      <anchor>gf9a8af6b7ab148a59221b1ef6972970f</anchor>
      <arglist>(const apr_dbd_driver_t *driver, apr_pool_t *pool, apr_dbd_t *handle, apr_dbd_results_t **res, apr_dbd_prepared_t *statement, int random,...)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>apr_dbd_pbquery</name>
      <anchorfile>group___a_p_r___util___d_b_d.html</anchorfile>
      <anchor>g6d003ae40358f631aeb33da0ead90906</anchor>
      <arglist>(const apr_dbd_driver_t *driver, apr_pool_t *pool, apr_dbd_t *handle, int *nrows, apr_dbd_prepared_t *statement, const void **args)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>apr_dbd_pbselect</name>
      <anchorfile>group___a_p_r___util___d_b_d.html</anchorfile>
      <anchor>g1f00c276bd580c35579bc4e366939571</anchor>
      <arglist>(const apr_dbd_driver_t *driver, apr_pool_t *pool, apr_dbd_t *handle, apr_dbd_results_t **res, apr_dbd_prepared_t *statement, int random, const void **args)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>apr_dbd_pvbquery</name>
      <anchorfile>group___a_p_r___util___d_b_d.html</anchorfile>
      <anchor>g8f5ae6c8ad0c8c724fa31a41ac41c3cd</anchor>
      <arglist>(const apr_dbd_driver_t *driver, apr_pool_t *pool, apr_dbd_t *handle, int *nrows, apr_dbd_prepared_t *statement,...)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>apr_dbd_pvbselect</name>
      <anchorfile>group___a_p_r___util___d_b_d.html</anchorfile>
      <anchor>gb6f0cd2e0f63f0c27beb967979b39e41</anchor>
      <arglist>(const apr_dbd_driver_t *driver, apr_pool_t *pool, apr_dbd_t *handle, apr_dbd_results_t **res, apr_dbd_prepared_t *statement, int random,...)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_dbd_datum_get</name>
      <anchorfile>group___a_p_r___util___d_b_d.html</anchorfile>
      <anchor>gb98c880aba6f6bc932490a2b9baa5264</anchor>
      <arglist>(const apr_dbd_driver_t *driver, apr_dbd_row_t *row, int col, apr_dbd_type_e type, void *data)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>apr_dbm.h</name>
    <path>/home/joe/src/apache/apr/apr-util/trunk/include/</path>
    <filename>apr__dbm_8h</filename>
    <class kind="struct">apr_datum_t</class>
    <member kind="define">
      <type>#define</type>
      <name>APR_DBM_READONLY</name>
      <anchorfile>group___a_p_r___util___d_b_m.html</anchorfile>
      <anchor>g12995c1e41e471974be9aec01247b742</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_DBM_READWRITE</name>
      <anchorfile>group___a_p_r___util___d_b_m.html</anchorfile>
      <anchor>gdfcec61581a0d0d8c726d3b253174b3a</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_DBM_RWCREATE</name>
      <anchorfile>group___a_p_r___util___d_b_m.html</anchorfile>
      <anchor>ga6ea90dc879edb56cb2325b45c49fd7c</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_DBM_RWTRUNC</name>
      <anchorfile>group___a_p_r___util___d_b_m.html</anchorfile>
      <anchor>g9390d754bc542cef506390d8e03f8c69</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>apr_dbm_t</type>
      <name>apr_dbm_t</name>
      <anchorfile>group___a_p_r___util___d_b_m.html</anchorfile>
      <anchor>g8cb9495772e83b6c9621742b78871ce7</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_dbm_open_ex</name>
      <anchorfile>group___a_p_r___util___d_b_m.html</anchorfile>
      <anchor>g50bec5831019310a0ad554cc8d5e0068</anchor>
      <arglist>(apr_dbm_t **dbm, const char *type, const char *name, apr_int32_t mode, apr_fileperms_t perm, apr_pool_t *cntxt)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_dbm_open</name>
      <anchorfile>group___a_p_r___util___d_b_m.html</anchorfile>
      <anchor>g6cbdd50fc93f881480b8a96ead202a03</anchor>
      <arglist>(apr_dbm_t **dbm, const char *name, apr_int32_t mode, apr_fileperms_t perm, apr_pool_t *cntxt)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>apr_dbm_close</name>
      <anchorfile>group___a_p_r___util___d_b_m.html</anchorfile>
      <anchor>g0e83207299fe586db289104137ac9dda</anchor>
      <arglist>(apr_dbm_t *dbm)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_dbm_fetch</name>
      <anchorfile>group___a_p_r___util___d_b_m.html</anchorfile>
      <anchor>gb99ae42a910d424ff143ddcc80345dcd</anchor>
      <arglist>(apr_dbm_t *dbm, apr_datum_t key, apr_datum_t *pvalue)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_dbm_store</name>
      <anchorfile>group___a_p_r___util___d_b_m.html</anchorfile>
      <anchor>gb05f022f2f4e3d0d18e35991c91bb6da</anchor>
      <arglist>(apr_dbm_t *dbm, apr_datum_t key, apr_datum_t value)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_dbm_delete</name>
      <anchorfile>group___a_p_r___util___d_b_m.html</anchorfile>
      <anchor>gbb7433ba5e429db24e6a6c9b2093b79e</anchor>
      <arglist>(apr_dbm_t *dbm, apr_datum_t key)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>apr_dbm_exists</name>
      <anchorfile>group___a_p_r___util___d_b_m.html</anchorfile>
      <anchor>g3e25adf0c35e4ea60beccc73a9cf8a91</anchor>
      <arglist>(apr_dbm_t *dbm, apr_datum_t key)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_dbm_firstkey</name>
      <anchorfile>group___a_p_r___util___d_b_m.html</anchorfile>
      <anchor>gbcc3791e85dc06a876a26e7735686613</anchor>
      <arglist>(apr_dbm_t *dbm, apr_datum_t *pkey)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_dbm_nextkey</name>
      <anchorfile>group___a_p_r___util___d_b_m.html</anchorfile>
      <anchor>g7391b506e6f857e04a4093aeda8d1c78</anchor>
      <arglist>(apr_dbm_t *dbm, apr_datum_t *pkey)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>apr_dbm_freedatum</name>
      <anchorfile>group___a_p_r___util___d_b_m.html</anchorfile>
      <anchor>gd98702224edbfc70a6a2877664a74b01</anchor>
      <arglist>(apr_dbm_t *dbm, apr_datum_t data)</arglist>
    </member>
    <member kind="function">
      <type>char *</type>
      <name>apr_dbm_geterror</name>
      <anchorfile>group___a_p_r___util___d_b_m.html</anchorfile>
      <anchor>g3a5f12fe099e57c12cd483ace5b1c130</anchor>
      <arglist>(apr_dbm_t *dbm, int *errcode, char *errbuf, apr_size_t errbufsize)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_dbm_get_usednames_ex</name>
      <anchorfile>group___a_p_r___util___d_b_m.html</anchorfile>
      <anchor>gf93eb50aca3b1b232c27875949145efb</anchor>
      <arglist>(apr_pool_t *pool, const char *type, const char *pathname, const char **used1, const char **used2)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>apr_dbm_get_usednames</name>
      <anchorfile>group___a_p_r___util___d_b_m.html</anchorfile>
      <anchor>g412a7f2a07cf91491d4d291790d9e4df</anchor>
      <arglist>(apr_pool_t *pool, const char *pathname, const char **used1, const char **used2)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>apr_hooks.h</name>
    <path>/home/joe/src/apache/apr/apr-util/trunk/include/</path>
    <filename>apr__hooks_8h</filename>
    <member kind="define">
      <type>#define</type>
      <name>APR_IMPLEMENT_HOOK_GET_PROTO</name>
      <anchorfile>group___a_p_r___util___hook.html</anchorfile>
      <anchor>g320e483e7f860f3c555b4528d9abf41e</anchor>
      <arglist>(ns, link, name)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_DECLARE_EXTERNAL_HOOK</name>
      <anchorfile>group___a_p_r___util___hook.html</anchorfile>
      <anchor>g3f78b97be611af19de205ac15cf23af1</anchor>
      <arglist>(ns, link, ret, name, args)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_HOOK_STRUCT</name>
      <anchorfile>group___a_p_r___util___hook.html</anchorfile>
      <anchor>g0dcdfb814242c191ac36c34a4efbda22</anchor>
      <arglist>(members)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_HOOK_LINK</name>
      <anchorfile>group___a_p_r___util___hook.html</anchorfile>
      <anchor>gf525d29d6a0c04b6c62efe62104cbf01</anchor>
      <arglist>(name)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_IMPLEMENT_EXTERNAL_HOOK_BASE</name>
      <anchorfile>group___a_p_r___util___hook.html</anchorfile>
      <anchor>gc989f7fa3c8eca3f96b360c182cc688b</anchor>
      <arglist>(ns, link, name)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_IMPLEMENT_EXTERNAL_HOOK_VOID</name>
      <anchorfile>group___a_p_r___util___hook.html</anchorfile>
      <anchor>g778461267f1d8dab08003c851f0b8e4e</anchor>
      <arglist>(ns, link, name, args_decl, args_use)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_IMPLEMENT_EXTERNAL_HOOK_RUN_ALL</name>
      <anchorfile>group___a_p_r___util___hook.html</anchorfile>
      <anchor>gc3546697395695fe99e11205d7b0b4bd</anchor>
      <arglist>(ns, link, ret, name, args_decl, args_use, ok, decline)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_IMPLEMENT_EXTERNAL_HOOK_RUN_FIRST</name>
      <anchorfile>group___a_p_r___util___hook.html</anchorfile>
      <anchor>g92f3dd3071316e47a9320d0b3e9286a5</anchor>
      <arglist>(ns, link, ret, name, args_decl, args_use, decline)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_HOOK_REALLY_FIRST</name>
      <anchorfile>group___a_p_r___util___hook.html</anchorfile>
      <anchor>g4476db33781bbd5684d2d4041b944cac</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_HOOK_FIRST</name>
      <anchorfile>group___a_p_r___util___hook.html</anchorfile>
      <anchor>gf804b3cc837502c4628f0d8ebcb29734</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_HOOK_MIDDLE</name>
      <anchorfile>group___a_p_r___util___hook.html</anchorfile>
      <anchor>g46a34c28993e99673a4ac75084d30ecf</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_HOOK_LAST</name>
      <anchorfile>group___a_p_r___util___hook.html</anchorfile>
      <anchor>g1a9e60ca7fac7f975225b0da25c7495b</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_HOOK_REALLY_LAST</name>
      <anchorfile>group___a_p_r___util___hook.html</anchorfile>
      <anchor>g6bd3bff1e40180d4942f16998e619a92</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>apr_hook_sort_register</name>
      <anchorfile>group___a_p_r___util___hook.html</anchorfile>
      <anchor>g054fff20db06876d08484c9a08bb583a</anchor>
      <arglist>(const char *szHookName, apr_array_header_t **aHooks)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>apr_hook_sort_all</name>
      <anchorfile>group___a_p_r___util___hook.html</anchorfile>
      <anchor>g3e1d0f1ab2a7ea2c7d4939dbb470f362</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>apr_hook_debug_show</name>
      <anchorfile>group___a_p_r___util___hook.html</anchorfile>
      <anchor>gdc0bd91825cce24ff6d3f78ff886d5a7</anchor>
      <arglist>(const char *szName, const char *const *aszPre, const char *const *aszSucc)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>apr_hook_deregister_all</name>
      <anchorfile>group___a_p_r___util___hook.html</anchorfile>
      <anchor>g9a91bafc283bd426398e395a695b0e35</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="variable">
      <type>apr_pool_t *</type>
      <name>apr_hook_global_pool</name>
      <anchorfile>group___a_p_r___util___hook.html</anchorfile>
      <anchor>gea189125cb8f0a439fe49574b0b1174e</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int</type>
      <name>apr_hook_debug_enabled</name>
      <anchorfile>group___a_p_r___util___hook.html</anchorfile>
      <anchor>gb9d601e0d6db632340e054edd1c0e464</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char *</type>
      <name>apr_hook_debug_current</name>
      <anchorfile>group___a_p_r___util___hook.html</anchorfile>
      <anchor>gad3051246a587accb41cba7b9567fb98</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>apr_ldap.h</name>
    <path>/home/joe/src/apache/apr/apr-util/trunk/include/</path>
    <filename>apr__ldap_8h</filename>
    <member kind="define">
      <type>#define</type>
      <name>APR_HAS_LDAP</name>
      <anchorfile>group___a_p_r___util___l_d_a_p.html</anchorfile>
      <anchor>g97ae9ae6ac09ee1d9837eed28afd764e</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_HAS_NETSCAPE_LDAPSDK</name>
      <anchorfile>group___a_p_r___util___l_d_a_p.html</anchorfile>
      <anchor>g46e2bf3cedfb64902534735b50846ce3</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_HAS_SOLARIS_LDAPSDK</name>
      <anchorfile>group___a_p_r___util___l_d_a_p.html</anchorfile>
      <anchor>g4d45b7c15daa1931c39c15ee4b448f65</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_HAS_NOVELL_LDAPSDK</name>
      <anchorfile>group___a_p_r___util___l_d_a_p.html</anchorfile>
      <anchor>g9aa49e25515d629a12ea959446733dcc</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_HAS_MOZILLA_LDAPSDK</name>
      <anchorfile>group___a_p_r___util___l_d_a_p.html</anchorfile>
      <anchor>g80299e03fe1a99180ec38a03272a377b</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_HAS_OPENLDAP_LDAPSDK</name>
      <anchorfile>group___a_p_r___util___l_d_a_p.html</anchorfile>
      <anchor>g84c39cde7657befcc27fc4cad3883cc2</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_HAS_MICROSOFT_LDAPSDK</name>
      <anchorfile>group___a_p_r___util___l_d_a_p.html</anchorfile>
      <anchor>gafd23814f14c16591776b6a8aef045b8</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_HAS_ZOS_LDAPSDK</name>
      <anchorfile>group___a_p_r___util___l_d_a_p.html</anchorfile>
      <anchor>g14eb849d0d7dca29924891f3aac9112f</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_HAS_OTHER_LDAPSDK</name>
      <anchorfile>group___a_p_r___util___l_d_a_p.html</anchorfile>
      <anchor>g1791ae13ab6f1c4a083eaf0d1102db99</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>apr_ldap_init.h</name>
    <path>/home/joe/src/apache/apr/apr-util/trunk/include/</path>
    <filename>apr__ldap__init_8h</filename>
    <includes id="apr__ldap_8h" name="apr_ldap.h" local="yes" imported="no">apr_ldap.h</includes>
  </compound>
  <compound kind="file">
    <name>apr_ldap_option.h</name>
    <path>/home/joe/src/apache/apr/apr-util/trunk/include/</path>
    <filename>apr__ldap__option_8h</filename>
    <includes id="apr__ldap_8h" name="apr_ldap.h" local="yes" imported="no">apr_ldap.h</includes>
  </compound>
  <compound kind="file">
    <name>apr_ldap_url.h</name>
    <path>/home/joe/src/apache/apr/apr-util/trunk/include/</path>
    <filename>apr__ldap__url_8h</filename>
  </compound>
  <compound kind="file">
    <name>apr_md4.h</name>
    <path>/home/joe/src/apache/apr/apr-util/trunk/include/</path>
    <filename>apr__md4_8h</filename>
    <includes id="apr__xlate_8h" name="apr_xlate.h" local="yes" imported="no">apr_xlate.h</includes>
    <class kind="struct">apr_md4_ctx_t</class>
    <member kind="define">
      <type>#define</type>
      <name>APR_MD4_DIGESTSIZE</name>
      <anchorfile>group___a_p_r___util___m_d4.html</anchorfile>
      <anchor>gca5ee7150775441ac40f8fca83e8c702</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>apr_md4_ctx_t</type>
      <name>apr_md4_ctx_t</name>
      <anchorfile>group___a_p_r___util___m_d4.html</anchorfile>
      <anchor>g0043f24cc9a1874370c412eda84dcef5</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_md4_init</name>
      <anchorfile>group___a_p_r___util___m_d4.html</anchorfile>
      <anchor>g7498a7caddfa12d8746cba054af560ca</anchor>
      <arglist>(apr_md4_ctx_t *context)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_md4_set_xlate</name>
      <anchorfile>group___a_p_r___util___m_d4.html</anchorfile>
      <anchor>g52436829302c7709575112d76e8c4baf</anchor>
      <arglist>(apr_md4_ctx_t *context, apr_xlate_t *xlate)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_md4_update</name>
      <anchorfile>group___a_p_r___util___m_d4.html</anchorfile>
      <anchor>g3ef01f727f939376645f764d6cbf2117</anchor>
      <arglist>(apr_md4_ctx_t *context, const unsigned char *input, apr_size_t inputLen)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_md4_final</name>
      <anchorfile>group___a_p_r___util___m_d4.html</anchorfile>
      <anchor>g3588dc327a19a69a4c0e8be19cf5e7ba</anchor>
      <arglist>(unsigned char digest[APR_MD4_DIGESTSIZE], apr_md4_ctx_t *context)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_md4</name>
      <anchorfile>group___a_p_r___util___m_d4.html</anchorfile>
      <anchor>gad5541e42634ddd63a21bedf1f3c6bf0</anchor>
      <arglist>(unsigned char digest[APR_MD4_DIGESTSIZE], const unsigned char *input, apr_size_t inputLen)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>apr_md5.h</name>
    <path>/home/joe/src/apache/apr/apr-util/trunk/include/</path>
    <filename>apr__md5_8h</filename>
    <includes id="apr__xlate_8h" name="apr_xlate.h" local="yes" imported="no">apr_xlate.h</includes>
    <class kind="struct">apr_md5_ctx_t</class>
    <member kind="define">
      <type>#define</type>
      <name>APR_MD5_DIGESTSIZE</name>
      <anchorfile>group___a_p_r___m_d5.html</anchorfile>
      <anchor>gec0d977a03646cb01a9ce4f156ad7049</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>apr_md5_ctx_t</type>
      <name>apr_md5_ctx_t</name>
      <anchorfile>group___a_p_r___m_d5.html</anchorfile>
      <anchor>gaba81a095b07466618f299b2d3d9b930</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_md5_init</name>
      <anchorfile>group___a_p_r___m_d5.html</anchorfile>
      <anchor>g8ab27e962e5ad4e01e74175bcfb52af8</anchor>
      <arglist>(apr_md5_ctx_t *context)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_md5_set_xlate</name>
      <anchorfile>group___a_p_r___m_d5.html</anchorfile>
      <anchor>g4a2f678278b9dad177259ce183da013b</anchor>
      <arglist>(apr_md5_ctx_t *context, apr_xlate_t *xlate)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_md5_update</name>
      <anchorfile>group___a_p_r___m_d5.html</anchorfile>
      <anchor>g2a73efa47b69ca260a41c851f39aa8e1</anchor>
      <arglist>(apr_md5_ctx_t *context, const void *input, apr_size_t inputLen)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_md5_final</name>
      <anchorfile>group___a_p_r___m_d5.html</anchorfile>
      <anchor>gec52db64037a58ed6030a12f1060610c</anchor>
      <arglist>(unsigned char digest[APR_MD5_DIGESTSIZE], apr_md5_ctx_t *context)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_md5</name>
      <anchorfile>group___a_p_r___m_d5.html</anchorfile>
      <anchor>g74079aa961c72d157508a2f31c18e289</anchor>
      <arglist>(unsigned char digest[APR_MD5_DIGESTSIZE], const void *input, apr_size_t inputLen)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_md5_encode</name>
      <anchorfile>group___a_p_r___m_d5.html</anchorfile>
      <anchor>g8e38aa9aede424191901b22fe34081f3</anchor>
      <arglist>(const char *password, const char *salt, char *result, apr_size_t nbytes)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_password_validate</name>
      <anchorfile>group___a_p_r___m_d5.html</anchorfile>
      <anchor>g5ddcb2395541250198fb7690c5d1fe0c</anchor>
      <arglist>(const char *passwd, const char *hash)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>apr_memcache.h</name>
    <path>/home/joe/src/apache/apr/apr-util/trunk/include/</path>
    <filename>apr__memcache_8h</filename>
    <includes id="apr__buckets_8h" name="apr_buckets.h" local="yes" imported="no">apr_buckets.h</includes>
    <includes id="apr__reslist_8h" name="apr_reslist.h" local="yes" imported="no">apr_reslist.h</includes>
    <class kind="struct">apr_memcache_server_t</class>
    <class kind="struct">apr_memcache_t</class>
    <class kind="struct">apr_memcache_value_t</class>
    <class kind="struct">apr_memcache_stats_t</class>
    <member kind="typedef">
      <type>apr_memcache_conn_t</type>
      <name>apr_memcache_conn_t</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>g9801000a269bfcff1ec39883f6d5d430</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>apr_memcache_server_t</type>
      <name>apr_memcache_server_t</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>gb135feeb15f3c31912b1e2795c8b7a4e</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <name>apr_memcache_server_status_t</name>
      <anchor>g7ddcdc3cff627563bbfca0d73cfe7a9f</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>APR_MC_SERVER_LIVE</name>
      <anchor>gg7ddcdc3cff627563bbfca0d73cfe7a9f19a18372aa0391203b9bd8eaa69f5cf2</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>APR_MC_SERVER_DEAD</name>
      <anchor>gg7ddcdc3cff627563bbfca0d73cfe7a9ff519062f8ee66c71ba1d703d0238edd5</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>APR_DECLARE</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>g852eaa5d78790ad83f9d2d3bdff81fcd</anchor>
      <arglist>(apr_uint32_t) apr_memcache_hash(const char *data</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>APR_DECLARE</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>g7f90210372c21721a45bc901b43601a4</anchor>
      <arglist>(apr_memcache_server_t *) apr_memcache_find_server_hash(apr_memcache_t *mc</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>APR_DECLARE</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>gb046f205cb0c9a3bb104fac8d26fb6cd</anchor>
      <arglist>(apr_status_t) apr_memcache_add_server(apr_memcache_t *mc</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>apr_memcache_add_multget_key</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>gebf4cda65413b926a202b8b7ed847930</anchor>
      <arglist>(apr_pool_t *data_pool, const char *key, apr_hash_t **values)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>apr_memcache_multgetp</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>gefe2f1487afe328403ccdae0e0024a6d</anchor>
      <arglist>(apr_memcache_t *mc, apr_pool_t *temp_pool, apr_pool_t *data_pool, apr_hash_t *values)</arglist>
    </member>
    <member kind="variable">
      <type>apr_size_t</type>
      <name>data_len</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>g5358156d49dab554db5ab905b15d0216</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const apr_uint32_t</type>
      <name>hash</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>g3b48afab70c4776ff981743c156e4c94</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apr_memcache_server_t *</type>
      <name>server</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>g70e14b5d9c89465e7d745e4c3f0a36f4</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char *</type>
      <name>host</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>ge299ba32612accbee735da724e29962a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char apr_port_t</type>
      <name>port</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>g2eb856cbbcc7de452bf099efaea5b36f</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apr_memcache_server_t *</type>
      <name>ms</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>gc0247808e1316fd01e8891b954053329</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apr_memcache_server_t *</type>
      <name>ms</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>gc0247808e1316fd01e8891b954053329</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char *</type>
      <name>host</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>ge299ba32612accbee735da724e29962a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char apr_port_t</type>
      <name>port</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>g2eb856cbbcc7de452bf099efaea5b36f</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char apr_port_t apr_uint32_t</type>
      <name>min</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>g12f1897ba538c47db8c5f62a206fb023</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char apr_port_t apr_uint32_t apr_uint32_t</type>
      <name>smax</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>gecdf08c5baa49e361b7ada8f5886847d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char apr_port_t apr_uint32_t apr_uint32_t apr_uint32_t</type>
      <name>max</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>g94e0986601932ae07e98004db4d56cf5</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char apr_port_t apr_uint32_t apr_uint32_t apr_uint32_t apr_uint32_t</type>
      <name>ttl</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>g8ddb5d824da3fe8793127f907c128289</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char apr_port_t apr_uint32_t apr_uint32_t apr_uint32_t apr_uint32_t apr_memcache_server_t **</type>
      <name>ns</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>g6a8b65c265c195b5bed28d892bba0942</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apr_uint16_t</type>
      <name>max_servers</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>gca02fb6d2ec6a0407b0e908d9eb30716</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apr_uint16_t apr_uint32_t</type>
      <name>flags</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>g260cfc81028e9876c5ed22308d03d908</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apr_uint16_t apr_uint32_t apr_memcache_t **</type>
      <name>mc</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>g32d07d9e6f57c586d9e7c1af38fe4947</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apr_pool_t *</type>
      <name>p</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>g1694c2a3936b5c65143864201d1e044a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apr_pool_t const char *</type>
      <name>key</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>g842ed24c713e30e6439c3720f05938b4</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apr_pool_t const char char **</type>
      <name>baton</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>g754f4a6138669d883ec4f53a316d3709</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apr_pool_t const char char apr_size_t *</type>
      <name>len</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>g7f0494f6390dda2989c702055127817c</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apr_pool_t const char char apr_size_t apr_uint16_t *</type>
      <name>flags</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>g34a8d49502a3a93e20fdae751cb24736</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char *</type>
      <name>key</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>g41c836d1cb3f7a0911a475bc7cf90078</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char char *</type>
      <name>baton</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>g656e5ea9f493d8fd6da0cdb3a49e4b4f</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char char const apr_size_t</type>
      <name>data_size</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>g644a4eb6ef4dc02779704ae36beecade</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char char const apr_size_t apr_uint32_t</type>
      <name>timeout</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>ga999c7ab703603bd5db7a7af6ac259e6</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char char const apr_size_t apr_uint32_t apr_uint16_t</type>
      <name>flags</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>g4662477274ca279e1c54e8b56e685549</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char *</type>
      <name>key</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>g41c836d1cb3f7a0911a475bc7cf90078</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char char *</type>
      <name>baton</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>g656e5ea9f493d8fd6da0cdb3a49e4b4f</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char char const apr_size_t</type>
      <name>data_size</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>g644a4eb6ef4dc02779704ae36beecade</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char char const apr_size_t apr_uint32_t</type>
      <name>timeout</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>ga999c7ab703603bd5db7a7af6ac259e6</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char char const apr_size_t apr_uint32_t apr_uint16_t</type>
      <name>flags</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>g4662477274ca279e1c54e8b56e685549</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char *</type>
      <name>key</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>g41c836d1cb3f7a0911a475bc7cf90078</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char char *</type>
      <name>data</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>gb0b2c3a8aeda505fd8b6253944e3a3d0</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char char const apr_size_t</type>
      <name>data_size</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>g644a4eb6ef4dc02779704ae36beecade</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char char const apr_size_t apr_uint32_t</type>
      <name>timeout</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>ga999c7ab703603bd5db7a7af6ac259e6</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char char const apr_size_t apr_uint32_t apr_uint16_t</type>
      <name>flags</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>g4662477274ca279e1c54e8b56e685549</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char *</type>
      <name>key</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>g41c836d1cb3f7a0911a475bc7cf90078</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char apr_uint32_t</type>
      <name>timeout</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>g6934dc8c7865171fa00221483a5c9d0d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char *</type>
      <name>key</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>g41c836d1cb3f7a0911a475bc7cf90078</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char apr_int32_t</type>
      <name>n</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>g280d09d604193c789b9f4741661de31b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char apr_int32_t apr_uint32_t *</type>
      <name>nv</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>g86dfdcfc2e078ca6f3e00170718ac905</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char *</type>
      <name>key</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>g41c836d1cb3f7a0911a475bc7cf90078</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char apr_int32_t</type>
      <name>n</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>g280d09d604193c789b9f4741661de31b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char apr_int32_t apr_uint32_t *</type>
      <name>new_value</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>gc56fb8cc07b8d62081c313eadf90a573</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apr_pool_t *</type>
      <name>p</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>g1694c2a3936b5c65143864201d1e044a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apr_pool_t char **</type>
      <name>baton</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>g1502f08f57b6b78fbb69423d0149bc18</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apr_pool_t *</type>
      <name>p</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>g1694c2a3936b5c65143864201d1e044a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apr_pool_t apr_memcache_stats_t **</type>
      <name>stats</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>gba635ba268417079099ea6a7c1513047</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>apr_optional.h</name>
    <path>/home/joe/src/apache/apr/apr-util/trunk/include/</path>
    <filename>apr__optional_8h</filename>
    <member kind="define">
      <type>#define</type>
      <name>APR_OPTIONAL_FN_TYPE</name>
      <anchorfile>group___a_p_r___util___opt.html</anchorfile>
      <anchor>gd16be52fdc9672d4cc2a689eb79cfe1b</anchor>
      <arglist>(name)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_DECLARE_OPTIONAL_FN</name>
      <anchorfile>group___a_p_r___util___opt.html</anchorfile>
      <anchor>gc7fd10ae0fc745fe48ee90345f0b730d</anchor>
      <arglist>(ret, name, args)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_REGISTER_OPTIONAL_FN</name>
      <anchorfile>group___a_p_r___util___opt.html</anchorfile>
      <anchor>g8a3f199b6460283af6164336c926be43</anchor>
      <arglist>(name)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_RETRIEVE_OPTIONAL_FN</name>
      <anchorfile>group___a_p_r___util___opt.html</anchorfile>
      <anchor>gd8c2989e7e89245137bc84a709443925</anchor>
      <arglist>(name)</arglist>
    </member>
    <member kind="typedef">
      <type>void(</type>
      <name>apr_opt_fn_t</name>
      <anchorfile>group___a_p_r___util___opt.html</anchorfile>
      <anchor>g98bef8c1c54c627088fba31dd8c6dd49</anchor>
      <arglist>)(void)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>apr_dynamic_fn_register</name>
      <anchorfile>group___a_p_r___util___opt.html</anchorfile>
      <anchor>gbd053d1d62e25b35c7214f280227dcc0</anchor>
      <arglist>(const char *szName, apr_opt_fn_t *pfn)</arglist>
    </member>
    <member kind="function">
      <type>apr_opt_fn_t *</type>
      <name>apr_dynamic_fn_retrieve</name>
      <anchorfile>group___a_p_r___util___opt.html</anchorfile>
      <anchor>gdcf7d8c80e8e2b7e2cb36dafa1620579</anchor>
      <arglist>(const char *szName)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>apr_optional_hooks.h</name>
    <path>/home/joe/src/apache/apr/apr-util/trunk/include/</path>
    <filename>apr__optional__hooks_8h</filename>
    <member kind="define">
      <type>#define</type>
      <name>APR_OPTIONAL_HOOK</name>
      <anchorfile>group___a_p_r___util___o_p_t___h_o_o_k.html</anchorfile>
      <anchor>g79437ac440ebae7cb2b3122ad6393f1a</anchor>
      <arglist>(ns, name, pfn, aszPre, aszSucc, nOrder)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_IMPLEMENT_OPTIONAL_HOOK_RUN_ALL</name>
      <anchorfile>group___a_p_r___util___o_p_t___h_o_o_k.html</anchorfile>
      <anchor>g5197eadc0461880e47687522da203afb</anchor>
      <arglist>(ns, link, ret, name, args_decl, args_use, ok, decline)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>apr_optional_hook_add</name>
      <anchorfile>group___a_p_r___util___o_p_t___h_o_o_k.html</anchorfile>
      <anchor>g7562a06f6d87425d7729a8dcdc3cc7fa</anchor>
      <arglist>(const char *szName, void(*pfn)(void), const char *const *aszPre, const char *const *aszSucc, int nOrder)</arglist>
    </member>
    <member kind="function">
      <type>apr_array_header_t *</type>
      <name>apr_optional_hook_get</name>
      <anchorfile>group___a_p_r___util___o_p_t___h_o_o_k.html</anchorfile>
      <anchor>gfbce21247691f1d8d785112071f870f8</anchor>
      <arglist>(const char *szName)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>apr_queue.h</name>
    <path>/home/joe/src/apache/apr/apr-util/trunk/include/</path>
    <filename>apr__queue_8h</filename>
    <member kind="typedef">
      <type>apr_queue_t</type>
      <name>apr_queue_t</name>
      <anchorfile>group___a_p_r___util___f_i_f_o.html</anchorfile>
      <anchor>g4fb6e57b49285fa45e4f598dbcd7d014</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_queue_create</name>
      <anchorfile>group___a_p_r___util___f_i_f_o.html</anchorfile>
      <anchor>g6c57799d9d0500dce636a537c743756f</anchor>
      <arglist>(apr_queue_t **queue, unsigned int queue_capacity, apr_pool_t *a)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_queue_push</name>
      <anchorfile>group___a_p_r___util___f_i_f_o.html</anchorfile>
      <anchor>g2606bc06b90c1e5b76abf55a3b2df722</anchor>
      <arglist>(apr_queue_t *queue, void *data)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_queue_pop</name>
      <anchorfile>group___a_p_r___util___f_i_f_o.html</anchorfile>
      <anchor>g65697cae0a71b83dd5b4c3e8f1735cc8</anchor>
      <arglist>(apr_queue_t *queue, void **data)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_queue_trypush</name>
      <anchorfile>group___a_p_r___util___f_i_f_o.html</anchorfile>
      <anchor>g290c91d5d42ac275e215d379ddba7722</anchor>
      <arglist>(apr_queue_t *queue, void *data)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_queue_trypop</name>
      <anchorfile>group___a_p_r___util___f_i_f_o.html</anchorfile>
      <anchor>g5642e67dbee42e1334924895b66d2192</anchor>
      <arglist>(apr_queue_t *queue, void **data)</arglist>
    </member>
    <member kind="function">
      <type>unsigned int</type>
      <name>apr_queue_size</name>
      <anchorfile>group___a_p_r___util___f_i_f_o.html</anchorfile>
      <anchor>g2c1ce13a013bfcc3b6ec5d7de37846c6</anchor>
      <arglist>(apr_queue_t *queue)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_queue_interrupt_all</name>
      <anchorfile>group___a_p_r___util___f_i_f_o.html</anchorfile>
      <anchor>g998296e3a87fbcf94928887696032a3c</anchor>
      <arglist>(apr_queue_t *queue)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_queue_term</name>
      <anchorfile>group___a_p_r___util___f_i_f_o.html</anchorfile>
      <anchor>g9a342118932c8918fa9e4307622e0617</anchor>
      <arglist>(apr_queue_t *queue)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>apr_reslist.h</name>
    <path>/home/joe/src/apache/apr/apr-util/trunk/include/</path>
    <filename>apr__reslist_8h</filename>
    <member kind="typedef">
      <type>apr_reslist_t</type>
      <name>apr_reslist_t</name>
      <anchorfile>group___a_p_r___util___r_l.html</anchorfile>
      <anchor>g2f25985a4666e98aacf0e1b8e3427f4c</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>apr_status_t(*)</type>
      <name>apr_reslist_constructor</name>
      <anchorfile>group___a_p_r___util___r_l.html</anchorfile>
      <anchor>g6e9eef7e4d2a8a576d2a71782a8a9b7b</anchor>
      <arglist>(void **resource, void *params, apr_pool_t *pool)</arglist>
    </member>
    <member kind="typedef">
      <type>apr_status_t(*)</type>
      <name>apr_reslist_destructor</name>
      <anchorfile>group___a_p_r___util___r_l.html</anchorfile>
      <anchor>g8b1565606af2d5bb91d8a65e010fb30c</anchor>
      <arglist>(void *resource, void *params, apr_pool_t *pool)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_reslist_create</name>
      <anchorfile>group___a_p_r___util___r_l.html</anchorfile>
      <anchor>g154edff9ed0928e2290aa81cd9ab56df</anchor>
      <arglist>(apr_reslist_t **reslist, int min, int smax, int hmax, apr_interval_time_t ttl, apr_reslist_constructor con, apr_reslist_destructor de, void *params, apr_pool_t *pool)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_reslist_destroy</name>
      <anchorfile>group___a_p_r___util___r_l.html</anchorfile>
      <anchor>g31829a18fcc0308624f07c2fd86467ac</anchor>
      <arglist>(apr_reslist_t *reslist)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_reslist_acquire</name>
      <anchorfile>group___a_p_r___util___r_l.html</anchorfile>
      <anchor>gc64594efd763d6bca2755c96e9341f8a</anchor>
      <arglist>(apr_reslist_t *reslist, void **resource)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_reslist_release</name>
      <anchorfile>group___a_p_r___util___r_l.html</anchorfile>
      <anchor>gbbd325ed5d2b0ae14fc914dd7da11ab3</anchor>
      <arglist>(apr_reslist_t *reslist, void *resource)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>apr_reslist_timeout_set</name>
      <anchorfile>group___a_p_r___util___r_l.html</anchorfile>
      <anchor>g0c7319e812f0322049aedc8bdf8675e7</anchor>
      <arglist>(apr_reslist_t *reslist, apr_interval_time_t timeout)</arglist>
    </member>
    <member kind="function">
      <type>apr_uint32_t</type>
      <name>apr_reslist_acquired_count</name>
      <anchorfile>group___a_p_r___util___r_l.html</anchorfile>
      <anchor>ge0a729e7e0bcd7badfcd57508c9de5fa</anchor>
      <arglist>(apr_reslist_t *reslist)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_reslist_invalidate</name>
      <anchorfile>group___a_p_r___util___r_l.html</anchorfile>
      <anchor>gf9bd72dad6a757187ccd3e5593d9a331</anchor>
      <arglist>(apr_reslist_t *reslist, void *resource)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>apr_rmm.h</name>
    <path>/home/joe/src/apache/apr/apr-util/trunk/include/</path>
    <filename>apr__rmm_8h</filename>
    <includes id="apr__anylock_8h" name="apr_anylock.h" local="yes" imported="no">apr_anylock.h</includes>
    <member kind="typedef">
      <type>apr_rmm_t</type>
      <name>apr_rmm_t</name>
      <anchorfile>group___a_p_r___util___r_m_m.html</anchorfile>
      <anchor>ge5eff8ce4f9b6c314bcb66da5ddee373</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>apr_size_t</type>
      <name>apr_rmm_off_t</name>
      <anchorfile>group___a_p_r___util___r_m_m.html</anchorfile>
      <anchor>g21f622eeacfc8032a9a60f9f619b8776</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_rmm_init</name>
      <anchorfile>group___a_p_r___util___r_m_m.html</anchorfile>
      <anchor>gac95f46ea30d105da794997c244f967c</anchor>
      <arglist>(apr_rmm_t **rmm, apr_anylock_t *lock, void *membuf, apr_size_t memsize, apr_pool_t *cont)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_rmm_destroy</name>
      <anchorfile>group___a_p_r___util___r_m_m.html</anchorfile>
      <anchor>g146a63d6b0283dbb8a73770a41d08fa8</anchor>
      <arglist>(apr_rmm_t *rmm)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_rmm_attach</name>
      <anchorfile>group___a_p_r___util___r_m_m.html</anchorfile>
      <anchor>ga78a320836add3af5361e1091152d091</anchor>
      <arglist>(apr_rmm_t **rmm, apr_anylock_t *lock, void *membuf, apr_pool_t *cont)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_rmm_detach</name>
      <anchorfile>group___a_p_r___util___r_m_m.html</anchorfile>
      <anchor>g3c38be767c654be8f53771ba4356498a</anchor>
      <arglist>(apr_rmm_t *rmm)</arglist>
    </member>
    <member kind="function">
      <type>apr_rmm_off_t</type>
      <name>apr_rmm_malloc</name>
      <anchorfile>group___a_p_r___util___r_m_m.html</anchorfile>
      <anchor>ga9a31df54bcc4c0af984c9bc2461827a</anchor>
      <arglist>(apr_rmm_t *rmm, apr_size_t reqsize)</arglist>
    </member>
    <member kind="function">
      <type>apr_rmm_off_t</type>
      <name>apr_rmm_realloc</name>
      <anchorfile>group___a_p_r___util___r_m_m.html</anchorfile>
      <anchor>g19bfda917948efe8115ae7f9f2a973f1</anchor>
      <arglist>(apr_rmm_t *rmm, void *entity, apr_size_t reqsize)</arglist>
    </member>
    <member kind="function">
      <type>apr_rmm_off_t</type>
      <name>apr_rmm_calloc</name>
      <anchorfile>group___a_p_r___util___r_m_m.html</anchorfile>
      <anchor>gc0036240d63edc38971f71b94c393b82</anchor>
      <arglist>(apr_rmm_t *rmm, apr_size_t reqsize)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_rmm_free</name>
      <anchorfile>group___a_p_r___util___r_m_m.html</anchorfile>
      <anchor>g35a889a6caf6002f5fd0c49990ab5e17</anchor>
      <arglist>(apr_rmm_t *rmm, apr_rmm_off_t entity)</arglist>
    </member>
    <member kind="function">
      <type>void *</type>
      <name>apr_rmm_addr_get</name>
      <anchorfile>group___a_p_r___util___r_m_m.html</anchorfile>
      <anchor>g88c36ee0ad4abdc643e01971c18fbdf8</anchor>
      <arglist>(apr_rmm_t *rmm, apr_rmm_off_t entity)</arglist>
    </member>
    <member kind="function">
      <type>apr_rmm_off_t</type>
      <name>apr_rmm_offset_get</name>
      <anchorfile>group___a_p_r___util___r_m_m.html</anchorfile>
      <anchor>g40cf37655a031012c9325be3cc278683</anchor>
      <arglist>(apr_rmm_t *rmm, void *entity)</arglist>
    </member>
    <member kind="function">
      <type>apr_size_t</type>
      <name>apr_rmm_overhead_get</name>
      <anchorfile>group___a_p_r___util___r_m_m.html</anchorfile>
      <anchor>g8fc0b723aa71eff2f81c9d05212c200c</anchor>
      <arglist>(int n)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>apr_sdbm.h</name>
    <path>/home/joe/src/apache/apr/apr-util/trunk/include/</path>
    <filename>apr__sdbm_8h</filename>
    <class kind="struct">apr_sdbm_datum_t</class>
    <member kind="define">
      <type>#define</type>
      <name>APR_SDBM_DIRFEXT</name>
      <anchorfile>group___a_p_r___util___d_b_m___s_d_b_m.html</anchorfile>
      <anchor>g0ff77f72468e824aad6e71c276fc4c73</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_SDBM_PAGFEXT</name>
      <anchorfile>group___a_p_r___util___d_b_m___s_d_b_m.html</anchorfile>
      <anchor>g48c662a7711370df6904848abce51d08</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_SDBM_INSERT</name>
      <anchorfile>group___a_p_r___util___d_b_m___s_d_b_m.html</anchorfile>
      <anchor>gf78d4b4d7ab410d18a12147aa67dc4f1</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_SDBM_REPLACE</name>
      <anchorfile>group___a_p_r___util___d_b_m___s_d_b_m.html</anchorfile>
      <anchor>g533f72464d7c6c1a79cf22c6c06f3fcf</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_SDBM_INSERTDUP</name>
      <anchorfile>group___a_p_r___util___d_b_m___s_d_b_m.html</anchorfile>
      <anchor>g23e98a293c9585a0dcdd8f5127e617c9</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>apr_sdbm_t</type>
      <name>apr_sdbm_t</name>
      <anchorfile>group___a_p_r___util___d_b_m___s_d_b_m.html</anchorfile>
      <anchor>gb1cd57a5984a7f350cd83705b6cc308a</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_sdbm_open</name>
      <anchorfile>group___a_p_r___util___d_b_m___s_d_b_m.html</anchorfile>
      <anchor>g5ede50dca933e162629cc43245036e41</anchor>
      <arglist>(apr_sdbm_t **db, const char *name, apr_int32_t mode, apr_fileperms_t perms, apr_pool_t *p)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_sdbm_close</name>
      <anchorfile>group___a_p_r___util___d_b_m___s_d_b_m.html</anchorfile>
      <anchor>gfa0176fc27d32191b8cfaed762301a95</anchor>
      <arglist>(apr_sdbm_t *db)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_sdbm_lock</name>
      <anchorfile>group___a_p_r___util___d_b_m___s_d_b_m.html</anchorfile>
      <anchor>g42bf926b5dacb9c755d3b7bace9c7492</anchor>
      <arglist>(apr_sdbm_t *db, int type)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_sdbm_unlock</name>
      <anchorfile>group___a_p_r___util___d_b_m___s_d_b_m.html</anchorfile>
      <anchor>g86480c8d22e51a35677f310ad81ee38d</anchor>
      <arglist>(apr_sdbm_t *db)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_sdbm_fetch</name>
      <anchorfile>group___a_p_r___util___d_b_m___s_d_b_m.html</anchorfile>
      <anchor>g92819c791734392bfaf0e5564a20ae18</anchor>
      <arglist>(apr_sdbm_t *db, apr_sdbm_datum_t *value, apr_sdbm_datum_t key)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_sdbm_store</name>
      <anchorfile>group___a_p_r___util___d_b_m___s_d_b_m.html</anchorfile>
      <anchor>ge6ba8345c1abe9f757841665b9daa866</anchor>
      <arglist>(apr_sdbm_t *db, apr_sdbm_datum_t key, apr_sdbm_datum_t value, int opt)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_sdbm_delete</name>
      <anchorfile>group___a_p_r___util___d_b_m___s_d_b_m.html</anchorfile>
      <anchor>g3de38cbc4c06d4fd9a5f3e08d0109af0</anchor>
      <arglist>(apr_sdbm_t *db, const apr_sdbm_datum_t key)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_sdbm_firstkey</name>
      <anchorfile>group___a_p_r___util___d_b_m___s_d_b_m.html</anchorfile>
      <anchor>g8f5c66960bcd43c75e271f757c110ff5</anchor>
      <arglist>(apr_sdbm_t *db, apr_sdbm_datum_t *key)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_sdbm_nextkey</name>
      <anchorfile>group___a_p_r___util___d_b_m___s_d_b_m.html</anchorfile>
      <anchor>g82eb237fc09febc743b14abb6a59fa45</anchor>
      <arglist>(apr_sdbm_t *db, apr_sdbm_datum_t *key)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>apr_sdbm_rdonly</name>
      <anchorfile>group___a_p_r___util___d_b_m___s_d_b_m.html</anchorfile>
      <anchor>g93b350d3aab8a8c41934a89da8a4351a</anchor>
      <arglist>(apr_sdbm_t *db)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>apr_sha1.h</name>
    <path>/home/joe/src/apache/apr/apr-util/trunk/include/</path>
    <filename>apr__sha1_8h</filename>
    <class kind="struct">apr_sha1_ctx_t</class>
    <member kind="define">
      <type>#define</type>
      <name>APR_SHA1_DIGESTSIZE</name>
      <anchorfile>apr__sha1_8h.html</anchorfile>
      <anchor>f0fe9d1e66116f5e6294853bc48c05c3</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_SHA1PW_ID</name>
      <anchorfile>apr__sha1_8h.html</anchorfile>
      <anchor>3fb50e6c3485c2c6b83df1d0c94d7310</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_SHA1PW_IDLEN</name>
      <anchorfile>apr__sha1_8h.html</anchorfile>
      <anchor>34e7271b953deb1da58117c961ed9301</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>apr_sha1_ctx_t</type>
      <name>apr_sha1_ctx_t</name>
      <anchorfile>apr__sha1_8h.html</anchorfile>
      <anchor>84282f1deffaaf8110befdfa354f1cc4</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>apr_sha1_base64</name>
      <anchorfile>apr__sha1_8h.html</anchorfile>
      <anchor>93e5ed2a12bd379a487cc9e74851a941</anchor>
      <arglist>(const char *clear, int len, char *out)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>apr_sha1_init</name>
      <anchorfile>apr__sha1_8h.html</anchorfile>
      <anchor>1353496de979eaf395da4bab2e7a149f</anchor>
      <arglist>(apr_sha1_ctx_t *context)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>apr_sha1_update</name>
      <anchorfile>apr__sha1_8h.html</anchorfile>
      <anchor>daf8bd2951c2fc1709a11812908756da</anchor>
      <arglist>(apr_sha1_ctx_t *context, const char *input, unsigned int inputLen)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>apr_sha1_update_binary</name>
      <anchorfile>apr__sha1_8h.html</anchorfile>
      <anchor>e71172351db173879e3510bb78cd4ff0</anchor>
      <arglist>(apr_sha1_ctx_t *context, const unsigned char *input, unsigned int inputLen)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>apr_sha1_final</name>
      <anchorfile>apr__sha1_8h.html</anchorfile>
      <anchor>516ddae9bec7f9a757d63bd25d5ad891</anchor>
      <arglist>(unsigned char digest[APR_SHA1_DIGESTSIZE], apr_sha1_ctx_t *context)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>apr_ssl.h</name>
    <path>/home/joe/src/apache/apr/apr-util/trunk/include/</path>
    <filename>apr__ssl_8h</filename>
    <member kind="typedef">
      <type>apr_ssl_factory</type>
      <name>apr_ssl_factory_t</name>
      <anchorfile>group___a_p_r___util___s_s_l.html</anchorfile>
      <anchor>gf45b4f4d0e8641392b25a9a505fe32e5</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>apr_ssl_socket</type>
      <name>apr_ssl_socket_t</name>
      <anchorfile>group___a_p_r___util___s_s_l.html</anchorfile>
      <anchor>g6732fe4958ad4a806ebd67b6de977176</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <name>apr_ssl_factory_type_e</name>
      <anchor>gd15e84ccd1d466840b71796fcf0e3dc9</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>APR_SSL_FACTORY_SERVER</name>
      <anchor>ggd15e84ccd1d466840b71796fcf0e3dc9f5b1e327a59a76868ccbf82f716230c8</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>APR_SSL_FACTORY_CLIENT</name>
      <anchor>ggd15e84ccd1d466840b71796fcf0e3dc93f22ef4b88ac3b1f8f8233d5c2e0ab54</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_ssl_factory_create</name>
      <anchorfile>group___a_p_r___util___s_s_l.html</anchorfile>
      <anchor>gefa85ebc22943c5f640cbd9514123ab9</anchor>
      <arglist>(apr_ssl_factory_t **, const char *, const char *, const char *, apr_ssl_factory_type_e, apr_pool_t *)</arglist>
    </member>
    <member kind="function">
      <type>const char *</type>
      <name>apr_ssl_library_name</name>
      <anchorfile>group___a_p_r___util___s_s_l.html</anchorfile>
      <anchor>gdf416172b3404bbbfd5fedcff31d1df6</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_ssl_socket_create</name>
      <anchorfile>group___a_p_r___util___s_s_l.html</anchorfile>
      <anchor>gf938ef9c97a5165bd8fcbd7b98736883</anchor>
      <arglist>(apr_ssl_socket_t **, int, int, int, apr_ssl_factory_t *, apr_pool_t *)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_ssl_socket_close</name>
      <anchorfile>group___a_p_r___util___s_s_l.html</anchorfile>
      <anchor>g8e0438971c5927ba79e078b73122917f</anchor>
      <arglist>(apr_ssl_socket_t *)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_ssl_socket_connect</name>
      <anchorfile>group___a_p_r___util___s_s_l.html</anchorfile>
      <anchor>g8baa1fa5149e8a3496f472e6469be8ad</anchor>
      <arglist>(apr_ssl_socket_t *, apr_sockaddr_t *)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_ssl_socket_send</name>
      <anchorfile>group___a_p_r___util___s_s_l.html</anchorfile>
      <anchor>g314e2920ee8d37e71ca452cb333b09df</anchor>
      <arglist>(apr_ssl_socket_t *, const char *, apr_size_t *)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_ssl_socket_recv</name>
      <anchorfile>group___a_p_r___util___s_s_l.html</anchorfile>
      <anchor>g5858e20f4919b9a242df32204dd1dc2f</anchor>
      <arglist>(apr_ssl_socket_t *, char *, apr_size_t *)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_ssl_socket_bind</name>
      <anchorfile>group___a_p_r___util___s_s_l.html</anchorfile>
      <anchor>gd48d442ad6dd57c5f56e68e177a98bac</anchor>
      <arglist>(apr_ssl_socket_t *, apr_sockaddr_t *)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_ssl_socket_listen</name>
      <anchorfile>group___a_p_r___util___s_s_l.html</anchorfile>
      <anchor>g12afb47b7668498c65d23a463aa30dd2</anchor>
      <arglist>(apr_ssl_socket_t *, apr_int32_t)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_ssl_socket_accept</name>
      <anchorfile>group___a_p_r___util___s_s_l.html</anchorfile>
      <anchor>ge4016cf9eb6fcba89be5f483f5cff333</anchor>
      <arglist>(apr_ssl_socket_t **, apr_ssl_socket_t *, apr_pool_t *)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_ssl_socket_raw_error</name>
      <anchorfile>group___a_p_r___util___s_s_l.html</anchorfile>
      <anchor>g1e1c9a97cded5c59dbd83b779b2d67f6</anchor>
      <arglist>(apr_ssl_socket_t *)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_pollset_add_ssl_socket</name>
      <anchorfile>group___a_p_r___util___s_s_l.html</anchorfile>
      <anchor>ge58304b5d70cbc0f7cdc3586763ccf29</anchor>
      <arglist>(apr_pollset_t *, apr_ssl_socket_t *)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_pollset_remove_ssl_socket</name>
      <anchorfile>group___a_p_r___util___s_s_l.html</anchorfile>
      <anchor>gef87870c16fdf8755bcdacea12ac277b</anchor>
      <arglist>(apr_ssl_socket_t *)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_ssl_socket_set_poll_events</name>
      <anchorfile>group___a_p_r___util___s_s_l.html</anchorfile>
      <anchor>g27399f8608d974c1662986c146d37136</anchor>
      <arglist>(apr_ssl_socket_t *, apr_int16_t)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>apr_strmatch.h</name>
    <path>/home/joe/src/apache/apr/apr-util/trunk/include/</path>
    <filename>apr__strmatch_8h</filename>
    <class kind="struct">apr_strmatch_pattern</class>
    <member kind="typedef">
      <type>apr_strmatch_pattern</type>
      <name>apr_strmatch_pattern</name>
      <anchorfile>group___a_p_r___util___str_match.html</anchorfile>
      <anchor>g1ebe05292f73000cf6973910d5e6c189</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>const char *</type>
      <name>apr_strmatch</name>
      <anchorfile>group___a_p_r___util___str_match.html</anchorfile>
      <anchor>g3481aa892c05ccc64ab9d5ab60f83328</anchor>
      <arglist>(const apr_strmatch_pattern *pattern, const char *s, apr_size_t slen)</arglist>
    </member>
    <member kind="function">
      <type>const apr_strmatch_pattern *</type>
      <name>apr_strmatch_precompile</name>
      <anchorfile>group___a_p_r___util___str_match.html</anchorfile>
      <anchor>g0745661741f86cb1d106d485cb0bf9ba</anchor>
      <arglist>(apr_pool_t *p, const char *s, int case_sensitive)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>apr_thread_pool.h</name>
    <path>/home/joe/src/apache/apr/apr-util/trunk/include/</path>
    <filename>apr__thread__pool_8h</filename>
    <member kind="define">
      <type>#define</type>
      <name>APR_THREAD_TASK_PRIORITY_LOWEST</name>
      <anchorfile>apr__thread__pool_8h.html</anchorfile>
      <anchor>2fcd8f1fed4f8d8de43e7bd8b9f75cb4</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_THREAD_TASK_PRIORITY_LOW</name>
      <anchorfile>apr__thread__pool_8h.html</anchorfile>
      <anchor>d4385895a0ae5064b484219dd0654517</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_THREAD_TASK_PRIORITY_NORMAL</name>
      <anchorfile>apr__thread__pool_8h.html</anchorfile>
      <anchor>2b40b1d5f75bf0223e87e0f12ac74979</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_THREAD_TASK_PRIORITY_HIGH</name>
      <anchorfile>apr__thread__pool_8h.html</anchorfile>
      <anchor>b5cf753321df8c762610e46eded55f5a</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_THREAD_TASK_PRIORITY_HIGHEST</name>
      <anchorfile>apr__thread__pool_8h.html</anchorfile>
      <anchor>2f1efb73bb8201434ecf02f034796393</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>apr_thread_pool</type>
      <name>apr_thread_pool_t</name>
      <anchorfile>apr__thread__pool_8h.html</anchorfile>
      <anchor>b839b481355f6917d81ca035c7e5d8f5</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>APR_DECLARE</name>
      <anchorfile>apr__thread__pool_8h.html</anchorfile>
      <anchor>53bacf9764119dda4efad267c2e01315</anchor>
      <arglist>(apr_status_t) apr_thread_pool_create(apr_thread_pool_t **me</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>APR_DECLARE</name>
      <anchorfile>apr__thread__pool_8h.html</anchorfile>
      <anchor>7769cb0076c7ec19e47906175998aba0</anchor>
      <arglist>(apr_size_t) apr_thread_pool_tasks_count(apr_thread_pool_t *me)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>apr_thread_pool_scheduled_tasks_count</name>
      <anchorfile>apr__thread__pool_8h.html</anchorfile>
      <anchor>af4aa06b7ee3b2063fe60202313ffb09</anchor>
      <arglist>(apr_thread_pool_t *me)</arglist>
    </member>
    <member kind="variable">
      <type>apr_size_t</type>
      <name>init_threads</name>
      <anchorfile>apr__thread__pool_8h.html</anchorfile>
      <anchor>b21df628f43c74b07fc1025a652d93d1</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apr_size_t apr_size_t</type>
      <name>max_threads</name>
      <anchorfile>apr__thread__pool_8h.html</anchorfile>
      <anchor>c857eff985ec60bf4a376771f3575069</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apr_size_t apr_size_t apr_pool_t *</type>
      <name>pool</name>
      <anchorfile>apr__thread__pool_8h.html</anchorfile>
      <anchor>c814ebd3ffe9bac84ef41b12c545949c</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apr_thread_start_t</type>
      <name>func</name>
      <anchorfile>apr__thread__pool_8h.html</anchorfile>
      <anchor>321e74e195b9ca145db23abebd2b5036</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apr_thread_start_t void *</type>
      <name>param</name>
      <anchorfile>apr__thread__pool_8h.html</anchorfile>
      <anchor>5d2b6e43b272481318f080dfedd5586b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apr_thread_start_t void apr_byte_t</type>
      <name>priority</name>
      <anchorfile>apr__thread__pool_8h.html</anchorfile>
      <anchor>173528b10f8b3989b3ff6f46571ce941</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apr_thread_start_t void apr_byte_t void *</type>
      <name>owner</name>
      <anchorfile>apr__thread__pool_8h.html</anchorfile>
      <anchor>c892738b0e294de419d773cdffc03b66</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apr_thread_start_t</type>
      <name>func</name>
      <anchorfile>apr__thread__pool_8h.html</anchorfile>
      <anchor>321e74e195b9ca145db23abebd2b5036</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apr_thread_start_t void *</type>
      <name>param</name>
      <anchorfile>apr__thread__pool_8h.html</anchorfile>
      <anchor>5d2b6e43b272481318f080dfedd5586b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apr_thread_start_t void apr_interval_time_t</type>
      <name>time</name>
      <anchorfile>apr__thread__pool_8h.html</anchorfile>
      <anchor>3dd401999c29b8ac393d739e68fdec31</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apr_thread_start_t void apr_interval_time_t void *</type>
      <name>owner</name>
      <anchorfile>apr__thread__pool_8h.html</anchorfile>
      <anchor>8cd7356d21677387a2dcf811a58c9a0d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apr_thread_start_t</type>
      <name>func</name>
      <anchorfile>apr__thread__pool_8h.html</anchorfile>
      <anchor>321e74e195b9ca145db23abebd2b5036</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apr_thread_start_t void *</type>
      <name>param</name>
      <anchorfile>apr__thread__pool_8h.html</anchorfile>
      <anchor>5d2b6e43b272481318f080dfedd5586b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apr_thread_start_t void apr_byte_t</type>
      <name>priority</name>
      <anchorfile>apr__thread__pool_8h.html</anchorfile>
      <anchor>173528b10f8b3989b3ff6f46571ce941</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apr_thread_start_t void apr_byte_t void *</type>
      <name>owner</name>
      <anchorfile>apr__thread__pool_8h.html</anchorfile>
      <anchor>c892738b0e294de419d773cdffc03b66</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>void *</type>
      <name>owner</name>
      <anchorfile>apr__thread__pool_8h.html</anchorfile>
      <anchor>fd44cc6444d7a8964b5d2b895364ad20</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apr_size_t</type>
      <name>cnt</name>
      <anchorfile>apr__thread__pool_8h.html</anchorfile>
      <anchor>d7859d77cc0fca947e8c4b46ca3064c0</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apr_size_t</type>
      <name>cnt</name>
      <anchorfile>apr__thread__pool_8h.html</anchorfile>
      <anchor>d7859d77cc0fca947e8c4b46ca3064c0</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apr_size_t</type>
      <name>val</name>
      <anchorfile>apr__thread__pool_8h.html</anchorfile>
      <anchor>bc25b3d058d31a3f89c44cee1a352066</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>void **</type>
      <name>owner</name>
      <anchorfile>apr__thread__pool_8h.html</anchorfile>
      <anchor>4c14295b09c25c31b9319beda8ce94ee</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>apr_uri.h</name>
    <path>/home/joe/src/apache/apr/apr-util/trunk/include/</path>
    <filename>apr__uri_8h</filename>
    <class kind="struct">apr_uri_t</class>
    <member kind="define">
      <type>#define</type>
      <name>APR_URI_FTP_DEFAULT_PORT</name>
      <anchorfile>group___a_p_r___util___u_r_i.html</anchorfile>
      <anchor>gb9580a0116c1fb0f8162f1f9ab2fd341</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_URI_SSH_DEFAULT_PORT</name>
      <anchorfile>group___a_p_r___util___u_r_i.html</anchorfile>
      <anchor>gedd084cdd64b0be5b588ddc4d6f0416d</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_URI_TELNET_DEFAULT_PORT</name>
      <anchorfile>group___a_p_r___util___u_r_i.html</anchorfile>
      <anchor>g1671c6188f245f7be5624147899da9cb</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_URI_GOPHER_DEFAULT_PORT</name>
      <anchorfile>group___a_p_r___util___u_r_i.html</anchorfile>
      <anchor>g7e8471723227df5c52d92460a2abb889</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_URI_HTTP_DEFAULT_PORT</name>
      <anchorfile>group___a_p_r___util___u_r_i.html</anchorfile>
      <anchor>g463777647d6208d178194adbbbbda5f3</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_URI_POP_DEFAULT_PORT</name>
      <anchorfile>group___a_p_r___util___u_r_i.html</anchorfile>
      <anchor>g137bc7ba4ce4b863feb8e436bc6bdd1f</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_URI_NNTP_DEFAULT_PORT</name>
      <anchorfile>group___a_p_r___util___u_r_i.html</anchorfile>
      <anchor>g0e4055b1902284394fd120e43dd3206e</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_URI_IMAP_DEFAULT_PORT</name>
      <anchorfile>group___a_p_r___util___u_r_i.html</anchorfile>
      <anchor>g53fc37da601746c25359849b27acfef3</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_URI_PROSPERO_DEFAULT_PORT</name>
      <anchorfile>group___a_p_r___util___u_r_i.html</anchorfile>
      <anchor>g7ca1b8eb28912516288a5a5ba42d1b11</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_URI_WAIS_DEFAULT_PORT</name>
      <anchorfile>group___a_p_r___util___u_r_i.html</anchorfile>
      <anchor>ge520a89f6202977fea5b195d2188cf06</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_URI_LDAP_DEFAULT_PORT</name>
      <anchorfile>group___a_p_r___util___u_r_i.html</anchorfile>
      <anchor>gc6ac7b320ccb58f3ad44b5ac490b7f51</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_URI_HTTPS_DEFAULT_PORT</name>
      <anchorfile>group___a_p_r___util___u_r_i.html</anchorfile>
      <anchor>g5ac25d3538574bc716d8127bdb202fbd</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_URI_RTSP_DEFAULT_PORT</name>
      <anchorfile>group___a_p_r___util___u_r_i.html</anchorfile>
      <anchor>gd66b83fc06352a6ba6219c5ae508901c</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_URI_SNEWS_DEFAULT_PORT</name>
      <anchorfile>group___a_p_r___util___u_r_i.html</anchorfile>
      <anchor>g3d3450d81b63780f5de2537ea4a8f96a</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_URI_ACAP_DEFAULT_PORT</name>
      <anchorfile>group___a_p_r___util___u_r_i.html</anchorfile>
      <anchor>ge8a436aedf533d466681ac56e85ccae2</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_URI_NFS_DEFAULT_PORT</name>
      <anchorfile>group___a_p_r___util___u_r_i.html</anchorfile>
      <anchor>gcd280bafe6f95366fa9582eb52cb74d1</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_URI_TIP_DEFAULT_PORT</name>
      <anchorfile>group___a_p_r___util___u_r_i.html</anchorfile>
      <anchor>g381d2bae735b236aa293a3399dcb17d5</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_URI_SIP_DEFAULT_PORT</name>
      <anchorfile>group___a_p_r___util___u_r_i.html</anchorfile>
      <anchor>g91c50d4625b831305cf85e6a3be9dde5</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_URI_UNP_OMITSITEPART</name>
      <anchorfile>group___a_p_r___util___u_r_i.html</anchorfile>
      <anchor>g13feebaa8e2a661a46beae0f8fb69551</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_URI_UNP_OMITUSER</name>
      <anchorfile>group___a_p_r___util___u_r_i.html</anchorfile>
      <anchor>g312de0239cc8f4b137b384f3d34f8ccb</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_URI_UNP_OMITPASSWORD</name>
      <anchorfile>group___a_p_r___util___u_r_i.html</anchorfile>
      <anchor>g7a65861c0175ebe3e163423dc874c3b8</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_URI_UNP_OMITUSERINFO</name>
      <anchorfile>group___a_p_r___util___u_r_i.html</anchorfile>
      <anchor>gb26098299959bf67a7becc479bb33e29</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_URI_UNP_REVEALPASSWORD</name>
      <anchorfile>group___a_p_r___util___u_r_i.html</anchorfile>
      <anchor>g67fbc86193b7fc0c6d4610807004b188</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_URI_UNP_OMITPATHINFO</name>
      <anchorfile>group___a_p_r___util___u_r_i.html</anchorfile>
      <anchor>gadcb0ef8ddcd2aacee457762454ec665</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_URI_UNP_OMITQUERY</name>
      <anchorfile>group___a_p_r___util___u_r_i.html</anchorfile>
      <anchor>g6ead3cdef58aad9828d6eece1a369146</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>apr_uri_t</type>
      <name>apr_uri_t</name>
      <anchorfile>group___a_p_r___util___u_r_i.html</anchorfile>
      <anchor>g612a571fda8dad43c9ccc835165b44b9</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>apr_port_t</type>
      <name>apr_uri_port_of_scheme</name>
      <anchorfile>group___a_p_r___util___u_r_i.html</anchorfile>
      <anchor>g7e1ba9317d01a213ba5aa1660cbfa5f9</anchor>
      <arglist>(const char *scheme_str)</arglist>
    </member>
    <member kind="function">
      <type>char *</type>
      <name>apr_uri_unparse</name>
      <anchorfile>group___a_p_r___util___u_r_i.html</anchorfile>
      <anchor>g063c2a75c575667771232b75e660ef8d</anchor>
      <arglist>(apr_pool_t *p, const apr_uri_t *uptr, unsigned flags)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_uri_parse</name>
      <anchorfile>group___a_p_r___util___u_r_i.html</anchorfile>
      <anchor>gfa0bad4fa506349667a7e95858a06d52</anchor>
      <arglist>(apr_pool_t *p, const char *uri, apr_uri_t *uptr)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_uri_parse_hostinfo</name>
      <anchorfile>group___a_p_r___util___u_r_i.html</anchorfile>
      <anchor>g8572e1555ca46fbebcb4bef0b70043b0</anchor>
      <arglist>(apr_pool_t *p, const char *hostinfo, apr_uri_t *uptr)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>apr_uuid.h</name>
    <path>/home/joe/src/apache/apr/apr-util/trunk/include/</path>
    <filename>apr__uuid_8h</filename>
    <class kind="struct">apr_uuid_t</class>
    <member kind="define">
      <type>#define</type>
      <name>APR_UUID_FORMATTED_LENGTH</name>
      <anchorfile>group___a_p_r___u_u_i_d.html</anchorfile>
      <anchor>g2ec982f0e604d3f98f7ee9fffde1bde7</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>apr_uuid_get</name>
      <anchorfile>group___a_p_r___u_u_i_d.html</anchorfile>
      <anchor>g88f83f266e153783fb95382c0d170ca9</anchor>
      <arglist>(apr_uuid_t *uuid)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>apr_uuid_format</name>
      <anchorfile>group___a_p_r___u_u_i_d.html</anchorfile>
      <anchor>g2b148ef181d5500e27a8aefd57dcb677</anchor>
      <arglist>(char *buffer, const apr_uuid_t *uuid)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_uuid_parse</name>
      <anchorfile>group___a_p_r___u_u_i_d.html</anchorfile>
      <anchor>gfb4812de1b199ba2c9101f112acc9aa4</anchor>
      <arglist>(apr_uuid_t *uuid, const char *uuid_str)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>apr_xlate.h</name>
    <path>/home/joe/src/apache/apr/apr-util/trunk/include/</path>
    <filename>apr__xlate_8h</filename>
    <member kind="define">
      <type>#define</type>
      <name>APR_DEFAULT_CHARSET</name>
      <anchorfile>group___a_p_r___x_l_a_t_e.html</anchorfile>
      <anchor>gbd7110ed72f4e2427d9f640062a0fc7e</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_LOCALE_CHARSET</name>
      <anchorfile>group___a_p_r___x_l_a_t_e.html</anchorfile>
      <anchor>g994245f7929287b3e5d9102783038c9e</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>apr_xlate_t</type>
      <name>apr_xlate_t</name>
      <anchorfile>group___a_p_r___x_l_a_t_e.html</anchorfile>
      <anchor>g2e43f9a7bf082375a2062a390d2bc4c4</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_xlate_open</name>
      <anchorfile>group___a_p_r___x_l_a_t_e.html</anchorfile>
      <anchor>g9a85ce73945a8644a344b188d7ee7b2c</anchor>
      <arglist>(apr_xlate_t **convset, const char *topage, const char *frompage, apr_pool_t *pool)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_xlate_sb_get</name>
      <anchorfile>group___a_p_r___x_l_a_t_e.html</anchorfile>
      <anchor>g6582b203b7d727ceb1739d8b468edf3e</anchor>
      <arglist>(apr_xlate_t *convset, int *onoff)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_xlate_conv_buffer</name>
      <anchorfile>group___a_p_r___x_l_a_t_e.html</anchorfile>
      <anchor>gd77888846e72130bdacf3f09dafb032f</anchor>
      <arglist>(apr_xlate_t *convset, const char *inbuf, apr_size_t *inbytes_left, char *outbuf, apr_size_t *outbytes_left)</arglist>
    </member>
    <member kind="function">
      <type>apr_int32_t</type>
      <name>apr_xlate_conv_byte</name>
      <anchorfile>group___a_p_r___x_l_a_t_e.html</anchorfile>
      <anchor>g562065db542b253a1b82798d08fa0538</anchor>
      <arglist>(apr_xlate_t *convset, unsigned char inchar)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_xlate_close</name>
      <anchorfile>group___a_p_r___x_l_a_t_e.html</anchorfile>
      <anchor>g5b5b3e59fb5377a94f8210783c76a595</anchor>
      <arglist>(apr_xlate_t *convset)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>apr_xml.h</name>
    <path>/home/joe/src/apache/apr/apr-util/trunk/include/</path>
    <filename>apr__xml_8h</filename>
    <namespace>Apache</namespace>
    <class kind="struct">apr_text</class>
    <class kind="struct">apr_text_header</class>
    <class kind="struct">apr_xml_attr</class>
    <class kind="struct">apr_xml_elem</class>
    <class kind="struct">apr_xml_doc</class>
    <member kind="define">
      <type>#define</type>
      <name>APR_XML_NS_DAV_ID</name>
      <anchorfile>group___a_p_r___util___x_m_l.html</anchorfile>
      <anchor>g4b12799e535116c05c40866f9881ff26</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_XML_NS_NONE</name>
      <anchorfile>group___a_p_r___util___x_m_l.html</anchorfile>
      <anchor>g189bc5b5bdd4ff22e92cf2ebe3c8b183</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_XML_NS_ERROR_BASE</name>
      <anchorfile>group___a_p_r___util___x_m_l.html</anchorfile>
      <anchor>ga18113c21d5035c8a75b348fa7ea4f4e</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_XML_NS_IS_ERROR</name>
      <anchorfile>group___a_p_r___util___x_m_l.html</anchorfile>
      <anchor>g4a53f376ee08fb9136d042446882dabf</anchor>
      <arglist>(e)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_XML_ELEM_IS_EMPTY</name>
      <anchorfile>group___a_p_r___util___x_m_l.html</anchorfile>
      <anchor>gb9de2eb1615437e5bb43d38fae0e0b0a</anchor>
      <arglist>(e)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_XML_X2T_FULL</name>
      <anchorfile>group___a_p_r___util___x_m_l.html</anchorfile>
      <anchor>g835b142aab53be5dae76320be483ebc2</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_XML_X2T_INNER</name>
      <anchorfile>group___a_p_r___util___x_m_l.html</anchorfile>
      <anchor>gd1f86b71a111a56019b2107e525e0846</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_XML_X2T_LANG_INNER</name>
      <anchorfile>group___a_p_r___util___x_m_l.html</anchorfile>
      <anchor>g5cbfc5f694421e01008e2676b45aa29d</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_XML_X2T_FULL_NS_LANG</name>
      <anchorfile>group___a_p_r___util___x_m_l.html</anchorfile>
      <anchor>g9b01beaf32a39279d2e066ae75946933</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_XML_GET_URI_ITEM</name>
      <anchorfile>group___a_p_r___util___x_m_l.html</anchorfile>
      <anchor>gbd99120feda9f8c5e6b8209f43f01d67</anchor>
      <arglist>(ary, i)</arglist>
    </member>
    <member kind="typedef">
      <type>apr_text</type>
      <name>apr_text</name>
      <anchorfile>group___a_p_r___util___x_m_l.html</anchorfile>
      <anchor>g2151b852b242e1d8e0ef0b12216884cb</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>apr_text_header</type>
      <name>apr_text_header</name>
      <anchorfile>group___a_p_r___util___x_m_l.html</anchorfile>
      <anchor>g0cbc2fc1d292873ed49490abe9bc44c5</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>apr_xml_attr</type>
      <name>apr_xml_attr</name>
      <anchorfile>group___a_p_r___util___x_m_l.html</anchorfile>
      <anchor>gc162efb6d441aa2a38a63fe7856bdb9e</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>apr_xml_elem</type>
      <name>apr_xml_elem</name>
      <anchorfile>group___a_p_r___util___x_m_l.html</anchorfile>
      <anchor>gb1bb002351dffa1905e12253d71becca</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>apr_xml_doc</type>
      <name>apr_xml_doc</name>
      <anchorfile>group___a_p_r___util___x_m_l.html</anchorfile>
      <anchor>g285ec448df650547ef79f842686c71dd</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>apr_xml_parser</type>
      <name>apr_xml_parser</name>
      <anchorfile>group___a_p_r___util___x_m_l.html</anchorfile>
      <anchor>gd8328a44c7358aebbd45cdf0397253cd</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>apr_text_append</name>
      <anchorfile>group___a_p_r___util___x_m_l.html</anchorfile>
      <anchor>g830e73772b197927fe33023a68037742</anchor>
      <arglist>(apr_pool_t *p, apr_text_header *hdr, const char *text)</arglist>
    </member>
    <member kind="function">
      <type>apr_xml_parser *</type>
      <name>apr_xml_parser_create</name>
      <anchorfile>group___a_p_r___util___x_m_l.html</anchorfile>
      <anchor>gda02ec28d7749957912d0511df6b4f9b</anchor>
      <arglist>(apr_pool_t *pool)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_xml_parse_file</name>
      <anchorfile>group___a_p_r___util___x_m_l.html</anchorfile>
      <anchor>g1c2376c0b60e2b9080420df2beaa3a57</anchor>
      <arglist>(apr_pool_t *p, apr_xml_parser **parser, apr_xml_doc **ppdoc, apr_file_t *xmlfd, apr_size_t buffer_length)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_xml_parser_feed</name>
      <anchorfile>group___a_p_r___util___x_m_l.html</anchorfile>
      <anchor>gaefd789d23ddb657d01be998f808e415</anchor>
      <arglist>(apr_xml_parser *parser, const char *data, apr_size_t len)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_xml_parser_done</name>
      <anchorfile>group___a_p_r___util___x_m_l.html</anchorfile>
      <anchor>g19c8232d9551ac41c392a3fa15844895</anchor>
      <arglist>(apr_xml_parser *parser, apr_xml_doc **pdoc)</arglist>
    </member>
    <member kind="function">
      <type>char *</type>
      <name>apr_xml_parser_geterror</name>
      <anchorfile>group___a_p_r___util___x_m_l.html</anchorfile>
      <anchor>g28a3791c51ab4ba15d0a544d9c4c905c</anchor>
      <arglist>(apr_xml_parser *parser, char *errbuf, apr_size_t errbufsize)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>apr_xml_to_text</name>
      <anchorfile>group___a_p_r___util___x_m_l.html</anchorfile>
      <anchor>gb95b65b64a5770435346905d609dc90d</anchor>
      <arglist>(apr_pool_t *p, const apr_xml_elem *elem, int style, apr_array_header_t *namespaces, int *ns_map, const char **pbuf, apr_size_t *psize)</arglist>
    </member>
    <member kind="function">
      <type>const char *</type>
      <name>apr_xml_empty_elem</name>
      <anchorfile>group___a_p_r___util___x_m_l.html</anchorfile>
      <anchor>g1a952361a96e05e14186c7de01949006</anchor>
      <arglist>(apr_pool_t *p, const apr_xml_elem *elem)</arglist>
    </member>
    <member kind="function">
      <type>const char *</type>
      <name>apr_xml_quote_string</name>
      <anchorfile>group___a_p_r___util___x_m_l.html</anchorfile>
      <anchor>g53c5be9052b94a59f07991c6094a0f42</anchor>
      <arglist>(apr_pool_t *p, const char *s, int quotes)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>apr_xml_quote_elem</name>
      <anchorfile>group___a_p_r___util___x_m_l.html</anchorfile>
      <anchor>g7742bdba87378eaa7a83d97555ae9130</anchor>
      <arglist>(apr_pool_t *p, apr_xml_elem *elem)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>apr_xml_insert_uri</name>
      <anchorfile>group___a_p_r___util___x_m_l.html</anchorfile>
      <anchor>gbcd20a5b9d2e9aec596f61207160b79a</anchor>
      <arglist>(apr_array_header_t *uri_array, const char *uri)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>apu_version.h</name>
    <path>/home/joe/src/apache/apr/apr-util/trunk/include/</path>
    <filename>apu__version_8h</filename>
    <member kind="define">
      <type>#define</type>
      <name>APU_MAJOR_VERSION</name>
      <anchorfile>apu__version_8h.html</anchorfile>
      <anchor>296c8dbcaa023537b3a7a9e9eb5c3163</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APU_MINOR_VERSION</name>
      <anchorfile>apu__version_8h.html</anchorfile>
      <anchor>738d61a958a36edc7ab8ea0a7f7b1e11</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APU_PATCH_VERSION</name>
      <anchorfile>apu__version_8h.html</anchorfile>
      <anchor>1d4e188ae6fdd2c0c5dad4b293fbfc2e</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APU_IS_DEV_VERSION</name>
      <anchorfile>apu__version_8h.html</anchorfile>
      <anchor>624cc5b6e3290567ad256891282d6291</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APU_IS_DEV_STRING</name>
      <anchorfile>apu__version_8h.html</anchorfile>
      <anchor>41b9f5f0e91cc9762541ce1facdab298</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APU_STRINGIFY</name>
      <anchorfile>apu__version_8h.html</anchorfile>
      <anchor>15521759e59b6edf14cc6cc54863f8bc</anchor>
      <arglist>(n)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APU_STRINGIFY_HELPER</name>
      <anchorfile>apu__version_8h.html</anchorfile>
      <anchor>2ddb5ee53db5f9acf998cd962577470e</anchor>
      <arglist>(n)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APU_VERSION_STRING</name>
      <anchorfile>apu__version_8h.html</anchorfile>
      <anchor>63f611e05ef5903d409c13ebb5804e64</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APU_VERSION_STRING_CSV</name>
      <anchorfile>apu__version_8h.html</anchorfile>
      <anchor>d602c4aa7b181ac3c55ecd8eb8e888ca</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>apu_version</name>
      <anchorfile>apu__version_8h.html</anchorfile>
      <anchor>c19305e4a11eafb8df1e33d3f2a3de4b</anchor>
      <arglist>(apr_version_t *pvsn)</arglist>
    </member>
    <member kind="function">
      <type>const char *</type>
      <name>apu_version_string</name>
      <anchorfile>apu__version_8h.html</anchorfile>
      <anchor>832551085a9fc7b567cfbf60a3563b31</anchor>
      <arglist>(void)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>apu_want.h</name>
    <path>/home/joe/src/apache/apr/apr-util/trunk/include/</path>
    <filename>apu__want_8h</filename>
  </compound>
  <compound kind="group">
    <name>APR_Util_Base64</name>
    <title>Base64 Encoding</title>
    <filename>group___a_p_r___util___base64.html</filename>
    <member kind="function">
      <type>int</type>
      <name>apr_base64_encode_len</name>
      <anchorfile>group___a_p_r___util___base64.html</anchorfile>
      <anchor>ga057204c7a32ca4fba300b91664403eb</anchor>
      <arglist>(int len)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>apr_base64_encode</name>
      <anchorfile>group___a_p_r___util___base64.html</anchorfile>
      <anchor>g046c7f12edd1a6bfa229a1f0a979fd78</anchor>
      <arglist>(char *coded_dst, const char *plain_src, int len_plain_src)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>apr_base64_encode_binary</name>
      <anchorfile>group___a_p_r___util___base64.html</anchorfile>
      <anchor>gb3646949fa3248f7c80adacca5750135</anchor>
      <arglist>(char *coded_dst, const unsigned char *plain_src, int len_plain_src)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>apr_base64_decode_len</name>
      <anchorfile>group___a_p_r___util___base64.html</anchorfile>
      <anchor>g14d7c3490149362dbb09bfa1c44c6159</anchor>
      <arglist>(const char *coded_src)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>apr_base64_decode</name>
      <anchorfile>group___a_p_r___util___base64.html</anchorfile>
      <anchor>gf35b1fcaa08f4f291e81b893bf7c5822</anchor>
      <arglist>(char *plain_dst, const char *coded_src)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>apr_base64_decode_binary</name>
      <anchorfile>group___a_p_r___util___base64.html</anchorfile>
      <anchor>g85648b30b638d45618aad6501ef91e40</anchor>
      <arglist>(unsigned char *plain_dst, const char *coded_src)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>APR_Util_Bucket_Brigades</name>
    <title>Bucket Brigades</title>
    <filename>group___a_p_r___util___bucket___brigades.html</filename>
    <class kind="struct">apr_bucket_type_t</class>
    <class kind="struct">apr_bucket</class>
    <class kind="struct">apr_bucket_brigade</class>
    <class kind="struct">apr_bucket_refcount</class>
    <class kind="struct">apr_bucket_heap</class>
    <class kind="struct">apr_bucket_pool</class>
    <class kind="struct">apr_bucket_mmap</class>
    <class kind="struct">apr_bucket_file</class>
    <class kind="union">apr_bucket_structs</class>
    <member kind="define">
      <type>#define</type>
      <name>APR_BUCKET_BUFF_SIZE</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>gf07a6e9e0f1824dba473e006fe235b01</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_BRIGADE_CHECK_CONSISTENCY</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>gcfade45f75165e946670b67625525af8</anchor>
      <arglist>(b)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_BUCKET_CHECK_CONSISTENCY</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g5043988f639cdde471a478ef0ffd50b8</anchor>
      <arglist>(e)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_BRIGADE_SENTINEL</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g55676273b34dc6c92967057a0e1789fb</anchor>
      <arglist>(b)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_BRIGADE_EMPTY</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>gf4bd27296485979250399d436cca1451</anchor>
      <arglist>(b)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_BRIGADE_FIRST</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g1713a11a5c04961ea1fca0d6773b2d6e</anchor>
      <arglist>(b)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_BRIGADE_LAST</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g8aba9ed9096a25d727b01bb63459a281</anchor>
      <arglist>(b)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_BRIGADE_INSERT_HEAD</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g7b2dfa82f2c38e8f40dca044c44a6209</anchor>
      <arglist>(b, e)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_BRIGADE_INSERT_TAIL</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g868dffd635ce2dbd715f9ebcdddd9bbe</anchor>
      <arglist>(b, e)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_BRIGADE_CONCAT</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g49beccfbe23f30cb21f73287b7edb94c</anchor>
      <arglist>(a, b)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_BRIGADE_PREPEND</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g5eb6d0312b4a2d106751383bdaddb8c2</anchor>
      <arglist>(a, b)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_BUCKET_INSERT_BEFORE</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g875b814756c7b894695ee0f4ee48e386</anchor>
      <arglist>(a, b)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_BUCKET_INSERT_AFTER</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g4a69b25ad02c7205dfb31b679993cec2</anchor>
      <arglist>(a, b)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_BUCKET_NEXT</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g52f9fd8b572940839043e56cc5215d24</anchor>
      <arglist>(e)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_BUCKET_PREV</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g5f0b7a368be7db5fe521113966f21db7</anchor>
      <arglist>(e)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_BUCKET_REMOVE</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g474aa518e9d447499978f0f33f638c79</anchor>
      <arglist>(e)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_BUCKET_INIT</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g62807e3c42ebaf0c7ca7b0f5352169b5</anchor>
      <arglist>(e)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_BUCKET_IS_METADATA</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g51de4889d492bdba6482df071e1e7b20</anchor>
      <arglist>(e)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_BUCKET_IS_FLUSH</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g1ce99772926ece2ae0a3c5539a9144db</anchor>
      <arglist>(e)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_BUCKET_IS_EOS</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g873276f0d8129a5b549fd204c57962c6</anchor>
      <arglist>(e)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_BUCKET_IS_FILE</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g8c413557c6e149c668ab756f366902ef</anchor>
      <arglist>(e)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_BUCKET_IS_PIPE</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g75e496497debf6beefb11d68feb33aea</anchor>
      <arglist>(e)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_BUCKET_IS_SOCKET</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>gdb6830a56ac43da909b04bd2435bd84c</anchor>
      <arglist>(e)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_BUCKET_IS_HEAP</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>gff8785fea8b20b12addb46cfec755087</anchor>
      <arglist>(e)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_BUCKET_IS_TRANSIENT</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>ga21bc955981e75df0fde97281d78295a</anchor>
      <arglist>(e)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_BUCKET_IS_IMMORTAL</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>ge1c2e5a41e6bad46caa7f1bcd94c7a3c</anchor>
      <arglist>(e)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_BUCKET_IS_MMAP</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>gcd8f26b880439dbbceda8d6514c108df</anchor>
      <arglist>(e)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_BUCKET_IS_POOL</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>gd403352637f5791429db99cec3a3114c</anchor>
      <arglist>(e)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_BUCKET_ALLOC_SIZE</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g8aa23a6c8ae12eb86fff2b6940992305</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>apr_bucket_destroy</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g08dd3c3b0dfbc4980024b5cbe5ef21c4</anchor>
      <arglist>(e)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>apr_bucket_delete</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g85f29c8023c0c8828ada41718613d2e8</anchor>
      <arglist>(e)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>apr_bucket_read</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g8e5b40655b940454e7c43886d4ead7d9</anchor>
      <arglist>(e, str, len, block)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>apr_bucket_setaside</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g80380462027f1f3d708d1f66a826ede1</anchor>
      <arglist>(e, p)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>apr_bucket_split</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>gc747716a3f3261d5a1ad7d887852fa08</anchor>
      <arglist>(e, point)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>apr_bucket_copy</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>gd2aed8a9000ed36e07dd4d3a42cd85d2</anchor>
      <arglist>(e, c)</arglist>
    </member>
    <member kind="typedef">
      <type>apr_bucket_brigade</type>
      <name>apr_bucket_brigade</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g1e85cecee7a3b35e1713423ad56d72dd</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>apr_bucket</type>
      <name>apr_bucket</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>gab31ae6d5598a2cc199b9a75361912fe</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>apr_bucket_alloc_t</type>
      <name>apr_bucket_alloc_t</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g070fee5f0cf4774a5a708602aeaa00bc</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>apr_bucket_type_t</type>
      <name>apr_bucket_type_t</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>gb671c52af157d10940a4b78aa1df8f90</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>apr_status_t(*)</type>
      <name>apr_brigade_flush</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g0ac2289234d5a275a4641bdf27a8f972</anchor>
      <arglist>(apr_bucket_brigade *bb, void *ctx)</arglist>
    </member>
    <member kind="typedef">
      <type>apr_bucket_refcount</type>
      <name>apr_bucket_refcount</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g83c7c56602c163acdd0d7509f671a78d</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>apr_bucket_heap</type>
      <name>apr_bucket_heap</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g2c684f54f50f41e21b114d14224529f1</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>apr_bucket_pool</type>
      <name>apr_bucket_pool</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>gcef65eca134ea81f6db1addab8a2ca67</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>apr_bucket_mmap</type>
      <name>apr_bucket_mmap</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g152912786f0cdf4e9bd763e1f5235eba</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>apr_bucket_file</type>
      <name>apr_bucket_file</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>gf14ceefc06e4689c23eb7f91e4380e23</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>apr_bucket_structs</type>
      <name>apr_bucket_structs</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g8ba0309f2a7c88e41e6b24e5045bad7c</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <name>apr_read_type_e</name>
      <anchor>g5ef12e1552824398cf3134ebe5fe1ba5</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>APR_BLOCK_READ</name>
      <anchor>gg5ef12e1552824398cf3134ebe5fe1ba59af8ad203f593fa4ae6df7aefc297b67</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>APR_NONBLOCK_READ</name>
      <anchor>gg5ef12e1552824398cf3134ebe5fe1ba53dc18612c7803798dbb446421bef53ba</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>apr_bucket_brigade *</type>
      <name>apr_brigade_create</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>ge6391b41fb52cf572f0ed72392e7cc5f</anchor>
      <arglist>(apr_pool_t *p, apr_bucket_alloc_t *list)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_brigade_destroy</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g8d1d90e229d0e8425e23179e38702a8b</anchor>
      <arglist>(apr_bucket_brigade *b)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_brigade_cleanup</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g47953ffefaf3c3b26847cb384588247c</anchor>
      <arglist>(void *data)</arglist>
    </member>
    <member kind="function">
      <type>apr_bucket_brigade *</type>
      <name>apr_brigade_split</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g6c58253544219af8c89a9d22ffacee26</anchor>
      <arglist>(apr_bucket_brigade *b, apr_bucket *e)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_brigade_partition</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>gcaab4c5fa66602686c4ad1a1ff0da7b6</anchor>
      <arglist>(apr_bucket_brigade *b, apr_off_t point, apr_bucket **after_point)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_brigade_length</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g7592a7a5293e32b8b581ce7d81968bc8</anchor>
      <arglist>(apr_bucket_brigade *bb, int read_all, apr_off_t *length)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_brigade_flatten</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>gb3683e145fafb15220be496e77c9111b</anchor>
      <arglist>(apr_bucket_brigade *bb, char *c, apr_size_t *len)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_brigade_pflatten</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g8cf3d55af2ae924c5637ed2250baa8b4</anchor>
      <arglist>(apr_bucket_brigade *bb, char **c, apr_size_t *len, apr_pool_t *pool)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_brigade_split_line</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g6203eab97431a9aa3777301ec25e357a</anchor>
      <arglist>(apr_bucket_brigade *bbOut, apr_bucket_brigade *bbIn, apr_read_type_e block, apr_off_t maxbytes)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_brigade_to_iovec</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>gebd10c7afc18d5ee105d86c8638158d2</anchor>
      <arglist>(apr_bucket_brigade *b, struct iovec *vec, int *nvec)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_brigade_vputstrs</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g2463542435a4a2ec4524225e05082705</anchor>
      <arglist>(apr_bucket_brigade *b, apr_brigade_flush flush, void *ctx, va_list va)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_brigade_write</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>gd9f6ec4084984506f99c7be3bc3e15e2</anchor>
      <arglist>(apr_bucket_brigade *b, apr_brigade_flush flush, void *ctx, const char *str, apr_size_t nbyte)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_brigade_writev</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g027e7c40c11f857957b8b95b758f75c1</anchor>
      <arglist>(apr_bucket_brigade *b, apr_brigade_flush flush, void *ctx, const struct iovec *vec, apr_size_t nvec)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_brigade_puts</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g30cdc932d2ea64881463de5a013b3eec</anchor>
      <arglist>(apr_bucket_brigade *bb, apr_brigade_flush flush, void *ctx, const char *str)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_brigade_putc</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g6a9fcbc7955169859562ee8614accd9e</anchor>
      <arglist>(apr_bucket_brigade *b, apr_brigade_flush flush, void *ctx, const char c)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_brigade_putstrs</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g05c36115a23e162815cc35f532d9b5e7</anchor>
      <arglist>(apr_bucket_brigade *b, apr_brigade_flush flush, void *ctx,...)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_brigade_printf</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>gb6c616490abe7ecea350dacc085297d3</anchor>
      <arglist>(apr_bucket_brigade *b, apr_brigade_flush flush, void *ctx, const char *fmt,...)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_brigade_vprintf</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g14032492513a841935c5c2e9051ccc65</anchor>
      <arglist>(apr_bucket_brigade *b, apr_brigade_flush flush, void *ctx, const char *fmt, va_list va)</arglist>
    </member>
    <member kind="function">
      <type>apr_bucket *</type>
      <name>apr_brigade_insert_file</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g6a2b211e2f23f623fc4e1a6bb16ca4fe</anchor>
      <arglist>(apr_bucket_brigade *bb, apr_file_t *f, apr_off_t start, apr_off_t len, apr_pool_t *p)</arglist>
    </member>
    <member kind="function">
      <type>apr_bucket_alloc_t *</type>
      <name>apr_bucket_alloc_create</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>ga418b24cfc354dc919a6842154f3fd90</anchor>
      <arglist>(apr_pool_t *p)</arglist>
    </member>
    <member kind="function">
      <type>apr_bucket_alloc_t *</type>
      <name>apr_bucket_alloc_create_ex</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g51d8f03f8d48f20e1a4027585adc4e68</anchor>
      <arglist>(apr_allocator_t *allocator)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>apr_bucket_alloc_destroy</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>gfa8882a7860537f61d91d1737e5c4c9f</anchor>
      <arglist>(apr_bucket_alloc_t *list)</arglist>
    </member>
    <member kind="function">
      <type>void *</type>
      <name>apr_bucket_alloc</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>gb254c96c2e523f5868c09d2ac2c062ac</anchor>
      <arglist>(apr_size_t size, apr_bucket_alloc_t *list)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>apr_bucket_free</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g12432b06513824816d867fefb9711868</anchor>
      <arglist>(void *block)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_bucket_setaside_noop</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g5cd4ee2c0b141e085d6daa25adc3f840</anchor>
      <arglist>(apr_bucket *data, apr_pool_t *pool)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_bucket_setaside_notimpl</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>gca5fc42472c09ccfaa4941818e2cf090</anchor>
      <arglist>(apr_bucket *data, apr_pool_t *pool)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_bucket_split_notimpl</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g7fa00d533df67814feec062050acb5bb</anchor>
      <arglist>(apr_bucket *data, apr_size_t point)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_bucket_copy_notimpl</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>gc6a1492d3906a58e0054b0c8196b28ca</anchor>
      <arglist>(apr_bucket *e, apr_bucket **c)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>apr_bucket_destroy_noop</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>gd46d087cf32369dc3ff1fcfdf68b9d95</anchor>
      <arglist>(void *data)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_bucket_simple_split</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>geef8f4c83b47c73c55b5586cc408e2e8</anchor>
      <arglist>(apr_bucket *b, apr_size_t point)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_bucket_simple_copy</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g9d7a889a0c5cc7d09fa26d395b493189</anchor>
      <arglist>(apr_bucket *a, apr_bucket **b)</arglist>
    </member>
    <member kind="function">
      <type>apr_bucket *</type>
      <name>apr_bucket_shared_make</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g3c2e1107521820ab795a2e213752fee2</anchor>
      <arglist>(apr_bucket *b, void *data, apr_off_t start, apr_size_t length)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>apr_bucket_shared_destroy</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>gbf84b299e8e7476a8132526f35923c40</anchor>
      <arglist>(void *data)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_bucket_shared_split</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g3a92dc8b4af7e33276c5b42d4e803fa8</anchor>
      <arglist>(apr_bucket *b, apr_size_t point)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_bucket_shared_copy</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g14b82b4e383254737967619c4340ff1d</anchor>
      <arglist>(apr_bucket *a, apr_bucket **b)</arglist>
    </member>
    <member kind="function">
      <type>apr_bucket *</type>
      <name>apr_bucket_eos_create</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>gb44d0ad8a3d6e49d1a84da534e692ae9</anchor>
      <arglist>(apr_bucket_alloc_t *list)</arglist>
    </member>
    <member kind="function">
      <type>apr_bucket *</type>
      <name>apr_bucket_eos_make</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>ge9f56bdf98be78a8fb27af76f6613d71</anchor>
      <arglist>(apr_bucket *b)</arglist>
    </member>
    <member kind="function">
      <type>apr_bucket *</type>
      <name>apr_bucket_flush_create</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>gc526b5961d44ba308dc3fde17f399862</anchor>
      <arglist>(apr_bucket_alloc_t *list)</arglist>
    </member>
    <member kind="function">
      <type>apr_bucket *</type>
      <name>apr_bucket_flush_make</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g2ea6dd5951e8c7b131c6f05fbd3a82e4</anchor>
      <arglist>(apr_bucket *b)</arglist>
    </member>
    <member kind="function">
      <type>apr_bucket *</type>
      <name>apr_bucket_immortal_create</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g8a4d5f9cbbaeedee5849715c0fd89524</anchor>
      <arglist>(const char *buf, apr_size_t nbyte, apr_bucket_alloc_t *list)</arglist>
    </member>
    <member kind="function">
      <type>apr_bucket *</type>
      <name>apr_bucket_immortal_make</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g57be0150e55539f5e9953b25f1e8e9b2</anchor>
      <arglist>(apr_bucket *b, const char *buf, apr_size_t nbyte)</arglist>
    </member>
    <member kind="function">
      <type>apr_bucket *</type>
      <name>apr_bucket_transient_create</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g2fb77e1aa8ddb3b1ab307494e18b7533</anchor>
      <arglist>(const char *buf, apr_size_t nbyte, apr_bucket_alloc_t *list)</arglist>
    </member>
    <member kind="function">
      <type>apr_bucket *</type>
      <name>apr_bucket_transient_make</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>gc587487d083659f18ff9e822671d741d</anchor>
      <arglist>(apr_bucket *b, const char *buf, apr_size_t nbyte)</arglist>
    </member>
    <member kind="function">
      <type>apr_bucket *</type>
      <name>apr_bucket_heap_create</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g3900923d436df2b2875f6232b786e033</anchor>
      <arglist>(const char *buf, apr_size_t nbyte, void(*free_func)(void *data), apr_bucket_alloc_t *list)</arglist>
    </member>
    <member kind="function">
      <type>apr_bucket *</type>
      <name>apr_bucket_heap_make</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>gd3904ae2eb5c824f100a7f1cc50f7315</anchor>
      <arglist>(apr_bucket *b, const char *buf, apr_size_t nbyte, void(*free_func)(void *data))</arglist>
    </member>
    <member kind="function">
      <type>apr_bucket *</type>
      <name>apr_bucket_pool_create</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g0dfed65563527d53662430d259336aeb</anchor>
      <arglist>(const char *buf, apr_size_t length, apr_pool_t *pool, apr_bucket_alloc_t *list)</arglist>
    </member>
    <member kind="function">
      <type>apr_bucket *</type>
      <name>apr_bucket_pool_make</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>gb9cdb85120a3b1556a34014cdbf4168e</anchor>
      <arglist>(apr_bucket *b, const char *buf, apr_size_t length, apr_pool_t *pool)</arglist>
    </member>
    <member kind="function">
      <type>apr_bucket *</type>
      <name>apr_bucket_mmap_create</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>ga9ad3b2763b2529aeda752b0758f8267</anchor>
      <arglist>(apr_mmap_t *mm, apr_off_t start, apr_size_t length, apr_bucket_alloc_t *list)</arglist>
    </member>
    <member kind="function">
      <type>apr_bucket *</type>
      <name>apr_bucket_mmap_make</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g86bafecc294063f4ffacacc037d56949</anchor>
      <arglist>(apr_bucket *b, apr_mmap_t *mm, apr_off_t start, apr_size_t length)</arglist>
    </member>
    <member kind="function">
      <type>apr_bucket *</type>
      <name>apr_bucket_socket_create</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g202f596c532528eb55613fa7a934dfa2</anchor>
      <arglist>(apr_socket_t *thissock, apr_bucket_alloc_t *list)</arglist>
    </member>
    <member kind="function">
      <type>apr_bucket *</type>
      <name>apr_bucket_socket_make</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>gdf474ab99206dc6afb2976cf44fa7583</anchor>
      <arglist>(apr_bucket *b, apr_socket_t *thissock)</arglist>
    </member>
    <member kind="function">
      <type>apr_bucket *</type>
      <name>apr_bucket_pipe_create</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g5788c7d7106efcef90cc8eac6e6f7826</anchor>
      <arglist>(apr_file_t *thispipe, apr_bucket_alloc_t *list)</arglist>
    </member>
    <member kind="function">
      <type>apr_bucket *</type>
      <name>apr_bucket_pipe_make</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>gc51915ac9885653a1405c6086f2af800</anchor>
      <arglist>(apr_bucket *b, apr_file_t *thispipe)</arglist>
    </member>
    <member kind="function">
      <type>apr_bucket *</type>
      <name>apr_bucket_file_create</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>ga8fedf2c13927414d34e3b77b46eafc8</anchor>
      <arglist>(apr_file_t *fd, apr_off_t offset, apr_size_t len, apr_pool_t *p, apr_bucket_alloc_t *list)</arglist>
    </member>
    <member kind="function">
      <type>apr_bucket *</type>
      <name>apr_bucket_file_make</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g3303f678b97c86c023a614ab65ad4cd1</anchor>
      <arglist>(apr_bucket *b, apr_file_t *fd, apr_off_t offset, apr_size_t len, apr_pool_t *p)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_bucket_file_enable_mmap</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>gf3caff67d3b6cc6166528ed7b03abb9f</anchor>
      <arglist>(apr_bucket *b, int enabled)</arglist>
    </member>
    <member kind="variable">
      <type>const apr_bucket_type_t</type>
      <name>apr_bucket_type_flush</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g02546431a51c18d5f59b1f2c2eb81fd8</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const apr_bucket_type_t</type>
      <name>apr_bucket_type_eos</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>gcbafc4f9831f24c9bcb8817a5bae8306</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const apr_bucket_type_t</type>
      <name>apr_bucket_type_file</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g062b60a82af96f68e3dd708e6f48fb1d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const apr_bucket_type_t</type>
      <name>apr_bucket_type_heap</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>gd16faf18a40f251d09128e1b58891fbd</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const apr_bucket_type_t</type>
      <name>apr_bucket_type_mmap</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>gf23216833215312bea6acae4cdd86aed</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const apr_bucket_type_t</type>
      <name>apr_bucket_type_pool</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>ga99fdbc30828e8e2168a5c6b929fa353</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const apr_bucket_type_t</type>
      <name>apr_bucket_type_pipe</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g49e70bb0383569e9b170924c3a0ab235</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const apr_bucket_type_t</type>
      <name>apr_bucket_type_immortal</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g8c56a396d94ba5e693f8ddd930c86a25</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const apr_bucket_type_t</type>
      <name>apr_bucket_type_transient</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>gd63b8f60ba9505b747df958951da24b1</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const apr_bucket_type_t</type>
      <name>apr_bucket_type_socket</name>
      <anchorfile>group___a_p_r___util___bucket___brigades.html</anchorfile>
      <anchor>g786fd03d89aa92cb3d0317846d7b32c4</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>APR_Util_Date</name>
    <title>Date routines</title>
    <filename>group___a_p_r___util___date.html</filename>
    <member kind="define">
      <type>#define</type>
      <name>APR_DATE_BAD</name>
      <anchorfile>group___a_p_r___util___date.html</anchorfile>
      <anchor>ga151e18dcfdadd1d137919175ef7bd96</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>apr_date_checkmask</name>
      <anchorfile>group___a_p_r___util___date.html</anchorfile>
      <anchor>g7c7475131bde438838bf57f7863c1678</anchor>
      <arglist>(const char *data, const char *mask)</arglist>
    </member>
    <member kind="function">
      <type>apr_time_t</type>
      <name>apr_date_parse_http</name>
      <anchorfile>group___a_p_r___util___date.html</anchorfile>
      <anchor>g03e62f436ae528733fc0c71c3895dc36</anchor>
      <arglist>(const char *date)</arglist>
    </member>
    <member kind="function">
      <type>apr_time_t</type>
      <name>apr_date_parse_rfc</name>
      <anchorfile>group___a_p_r___util___date.html</anchorfile>
      <anchor>gcef698361f924851a2c6940d9ef5ceca</anchor>
      <arglist>(const char *date)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>APR_Util_DBD</name>
    <title>DBD routines</title>
    <filename>group___a_p_r___util___d_b_d.html</filename>
    <member kind="define">
      <type>#define</type>
      <name>APR_DBD_TRANSACTION_COMMIT</name>
      <anchorfile>group___a_p_r___util___d_b_d.html</anchorfile>
      <anchor>g8e5ca5a4c5479a44faf9f2825b827044</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_DBD_TRANSACTION_ROLLBACK</name>
      <anchorfile>group___a_p_r___util___d_b_d.html</anchorfile>
      <anchor>g0734f735259dd95d65a6038b65bcc194</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_DBD_TRANSACTION_IGNORE_ERRORS</name>
      <anchorfile>group___a_p_r___util___d_b_d.html</anchorfile>
      <anchor>g8ddbc7080f2faa7c0510ba4f9fd1aa1f</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>apr_dbd_driver_t</type>
      <name>apr_dbd_driver_t</name>
      <anchorfile>group___a_p_r___util___d_b_d.html</anchorfile>
      <anchor>g46570be872f51bf03a121c6a4d42f343</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>apr_dbd_t</type>
      <name>apr_dbd_t</name>
      <anchorfile>group___a_p_r___util___d_b_d.html</anchorfile>
      <anchor>g865a5e3cf75216b7b59105aa691806a5</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>apr_dbd_transaction_t</type>
      <name>apr_dbd_transaction_t</name>
      <anchorfile>group___a_p_r___util___d_b_d.html</anchorfile>
      <anchor>gb63baa9e8a7c4e2a700a3bdc842a6405</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>apr_dbd_results_t</type>
      <name>apr_dbd_results_t</name>
      <anchorfile>group___a_p_r___util___d_b_d.html</anchorfile>
      <anchor>gb8a533bb665756fa711fbe3ff7fb6d0b</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>apr_dbd_row_t</type>
      <name>apr_dbd_row_t</name>
      <anchorfile>group___a_p_r___util___d_b_d.html</anchorfile>
      <anchor>g9c6eb2d2d432b82dec9fb3e6d98fb376</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>apr_dbd_prepared_t</type>
      <name>apr_dbd_prepared_t</name>
      <anchorfile>group___a_p_r___util___d_b_d.html</anchorfile>
      <anchor>g8aee259b115eb0bd041de73f65a68ce2</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <name>apr_dbd_type_e</name>
      <anchor>gfd17f821a3aaf8194b2866439826e3ca</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>APR_DBD_TYPE_TINY</name>
      <anchor>ggfd17f821a3aaf8194b2866439826e3cabef1e971e17a86470be792fdeedb4c9a</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>APR_DBD_TYPE_UTINY</name>
      <anchor>ggfd17f821a3aaf8194b2866439826e3cafb688b7a74245eecb8dceca55089f41d</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>APR_DBD_TYPE_SHORT</name>
      <anchor>ggfd17f821a3aaf8194b2866439826e3cae0d4a732098074362d6693611d3b4c5a</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>APR_DBD_TYPE_USHORT</name>
      <anchor>ggfd17f821a3aaf8194b2866439826e3ca788a9df38d2a3164b307a511e2a7e695</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>APR_DBD_TYPE_INT</name>
      <anchor>ggfd17f821a3aaf8194b2866439826e3caeb84a5980ca95dba9a849fb3878d6a3b</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>APR_DBD_TYPE_UINT</name>
      <anchor>ggfd17f821a3aaf8194b2866439826e3ca4f742de6028d01556dea20be409a998b</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>APR_DBD_TYPE_LONG</name>
      <anchor>ggfd17f821a3aaf8194b2866439826e3ca829dffc7f8e39d175cbd6289808bec4c</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>APR_DBD_TYPE_ULONG</name>
      <anchor>ggfd17f821a3aaf8194b2866439826e3ca16d7381b12e784d476498ecea1e62556</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>APR_DBD_TYPE_LONGLONG</name>
      <anchor>ggfd17f821a3aaf8194b2866439826e3ca3656373bcf0a066072bb8a4bb24424cc</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>APR_DBD_TYPE_ULONGLONG</name>
      <anchor>ggfd17f821a3aaf8194b2866439826e3ca4e352f61af22456e99e7fb371cc973c1</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>APR_DBD_TYPE_FLOAT</name>
      <anchor>ggfd17f821a3aaf8194b2866439826e3cabc264fdefbf8933b8e87a46cd20d3cf7</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>APR_DBD_TYPE_DOUBLE</name>
      <anchor>ggfd17f821a3aaf8194b2866439826e3cae4eba0afbbb932176213f1031bfa6b15</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>APR_DBD_TYPE_STRING</name>
      <anchor>ggfd17f821a3aaf8194b2866439826e3ca5645d699570b30484f61b4c15eedbc42</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>APR_DBD_TYPE_TEXT</name>
      <anchor>ggfd17f821a3aaf8194b2866439826e3ca22b3086337eff24e821b1d2c8963d537</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>APR_DBD_TYPE_TIME</name>
      <anchor>ggfd17f821a3aaf8194b2866439826e3cae2548b38b38dc1cb37f2f018537f5891</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>APR_DBD_TYPE_DATE</name>
      <anchor>ggfd17f821a3aaf8194b2866439826e3ca9cd5bcbb71647f02ea5ec5abcdf15563</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>APR_DBD_TYPE_DATETIME</name>
      <anchor>ggfd17f821a3aaf8194b2866439826e3ca93f650a5f12155fbe6ed43a60e793b5e</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>APR_DBD_TYPE_TIMESTAMP</name>
      <anchor>ggfd17f821a3aaf8194b2866439826e3ca870a86c5f8d620a4d4a573660a3573ee</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>APR_DBD_TYPE_ZTIMESTAMP</name>
      <anchor>ggfd17f821a3aaf8194b2866439826e3ca69e90e396d90cbfd7012c4afa7fadb4c</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>APR_DBD_TYPE_BLOB</name>
      <anchor>ggfd17f821a3aaf8194b2866439826e3ca9938baebc174dd56b56dcb90da384f08</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>APR_DBD_TYPE_CLOB</name>
      <anchor>ggfd17f821a3aaf8194b2866439826e3ca006f34e981c64a0b462f41d4be240752</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>APR_DBD_TYPE_NULL</name>
      <anchor>ggfd17f821a3aaf8194b2866439826e3ca15af2d239ca828a2c0992f528d45178c</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_dbd_init</name>
      <anchorfile>group___a_p_r___util___d_b_d.html</anchorfile>
      <anchor>g65c75cd4fdce468b4c384874e374423b</anchor>
      <arglist>(apr_pool_t *pool)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_dbd_get_driver</name>
      <anchorfile>group___a_p_r___util___d_b_d.html</anchorfile>
      <anchor>gbf5f24e2c8ff9a78ab4a254b3d6bbf44</anchor>
      <arglist>(apr_pool_t *pool, const char *name, const apr_dbd_driver_t **driver)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_dbd_open</name>
      <anchorfile>group___a_p_r___util___d_b_d.html</anchorfile>
      <anchor>g28f424d11d2e6543daeeec319234bf8c</anchor>
      <arglist>(const apr_dbd_driver_t *driver, apr_pool_t *pool, const char *params, apr_dbd_t **handle)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_dbd_close</name>
      <anchorfile>group___a_p_r___util___d_b_d.html</anchorfile>
      <anchor>g6b615b8a1ea2f15645e6da2e328e3b2b</anchor>
      <arglist>(const apr_dbd_driver_t *driver, apr_dbd_t *handle)</arglist>
    </member>
    <member kind="function">
      <type>const char *</type>
      <name>apr_dbd_name</name>
      <anchorfile>group___a_p_r___util___d_b_d.html</anchorfile>
      <anchor>g31a26096673464b57a28656b151e6b7c</anchor>
      <arglist>(const apr_dbd_driver_t *driver)</arglist>
    </member>
    <member kind="function">
      <type>void *</type>
      <name>apr_dbd_native_handle</name>
      <anchorfile>group___a_p_r___util___d_b_d.html</anchorfile>
      <anchor>gb83a44da154700ee31ca25a38739e0fe</anchor>
      <arglist>(const apr_dbd_driver_t *driver, apr_dbd_t *handle)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>apr_dbd_check_conn</name>
      <anchorfile>group___a_p_r___util___d_b_d.html</anchorfile>
      <anchor>ge7651453a0a89035d44e8aa0fd7b8948</anchor>
      <arglist>(const apr_dbd_driver_t *driver, apr_pool_t *pool, apr_dbd_t *handle)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>apr_dbd_set_dbname</name>
      <anchorfile>group___a_p_r___util___d_b_d.html</anchorfile>
      <anchor>g617861675bf28b0a2a04e66507710ddd</anchor>
      <arglist>(const apr_dbd_driver_t *driver, apr_pool_t *pool, apr_dbd_t *handle, const char *name)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>apr_dbd_transaction_start</name>
      <anchorfile>group___a_p_r___util___d_b_d.html</anchorfile>
      <anchor>g0ed5674f553e53e2a00db77a332e9296</anchor>
      <arglist>(const apr_dbd_driver_t *driver, apr_pool_t *pool, apr_dbd_t *handle, apr_dbd_transaction_t **trans)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>apr_dbd_transaction_end</name>
      <anchorfile>group___a_p_r___util___d_b_d.html</anchorfile>
      <anchor>g76a1a512c08b60b61c573afa39ba784a</anchor>
      <arglist>(const apr_dbd_driver_t *driver, apr_pool_t *pool, apr_dbd_transaction_t *trans)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>apr_dbd_transaction_mode_get</name>
      <anchorfile>group___a_p_r___util___d_b_d.html</anchorfile>
      <anchor>g4b65e6699cab49587d045f60c801f578</anchor>
      <arglist>(const apr_dbd_driver_t *driver, apr_dbd_transaction_t *trans)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>apr_dbd_transaction_mode_set</name>
      <anchorfile>group___a_p_r___util___d_b_d.html</anchorfile>
      <anchor>g30d4bba4826c58161e120aaa350fad19</anchor>
      <arglist>(const apr_dbd_driver_t *driver, apr_dbd_transaction_t *trans, int mode)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>apr_dbd_query</name>
      <anchorfile>group___a_p_r___util___d_b_d.html</anchorfile>
      <anchor>g48b2639510cc7398dc2e00d461934e0d</anchor>
      <arglist>(const apr_dbd_driver_t *driver, apr_dbd_t *handle, int *nrows, const char *statement)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>apr_dbd_select</name>
      <anchorfile>group___a_p_r___util___d_b_d.html</anchorfile>
      <anchor>g69ba5de5c3bc108ff13c4de5d8c879bd</anchor>
      <arglist>(const apr_dbd_driver_t *driver, apr_pool_t *pool, apr_dbd_t *handle, apr_dbd_results_t **res, const char *statement, int random)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>apr_dbd_num_cols</name>
      <anchorfile>group___a_p_r___util___d_b_d.html</anchorfile>
      <anchor>gbd8a416d60cbcd4b8cc4a8509c3039f4</anchor>
      <arglist>(const apr_dbd_driver_t *driver, apr_dbd_results_t *res)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>apr_dbd_num_tuples</name>
      <anchorfile>group___a_p_r___util___d_b_d.html</anchorfile>
      <anchor>g1855a09e38b6324e364579d0e75f767c</anchor>
      <arglist>(const apr_dbd_driver_t *driver, apr_dbd_results_t *res)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>apr_dbd_get_row</name>
      <anchorfile>group___a_p_r___util___d_b_d.html</anchorfile>
      <anchor>gff7f18d3427ab85e6f740becef0d7ad5</anchor>
      <arglist>(const apr_dbd_driver_t *driver, apr_pool_t *pool, apr_dbd_results_t *res, apr_dbd_row_t **row, int rownum)</arglist>
    </member>
    <member kind="function">
      <type>const char *</type>
      <name>apr_dbd_get_entry</name>
      <anchorfile>group___a_p_r___util___d_b_d.html</anchorfile>
      <anchor>g41ad06f5c9436451589aaa0cbbb0cad8</anchor>
      <arglist>(const apr_dbd_driver_t *driver, apr_dbd_row_t *row, int col)</arglist>
    </member>
    <member kind="function">
      <type>const char *</type>
      <name>apr_dbd_get_name</name>
      <anchorfile>group___a_p_r___util___d_b_d.html</anchorfile>
      <anchor>g3d86bd9523f4db1b87418e1f310bdf3b</anchor>
      <arglist>(const apr_dbd_driver_t *driver, apr_dbd_results_t *res, int col)</arglist>
    </member>
    <member kind="function">
      <type>const char *</type>
      <name>apr_dbd_error</name>
      <anchorfile>group___a_p_r___util___d_b_d.html</anchorfile>
      <anchor>g45831b321056c86a7eae233b2d2bbeb7</anchor>
      <arglist>(const apr_dbd_driver_t *driver, apr_dbd_t *handle, int errnum)</arglist>
    </member>
    <member kind="function">
      <type>const char *</type>
      <name>apr_dbd_escape</name>
      <anchorfile>group___a_p_r___util___d_b_d.html</anchorfile>
      <anchor>g463bd686a0b381da2966b72eccf45a0c</anchor>
      <arglist>(const apr_dbd_driver_t *driver, apr_pool_t *pool, const char *string, apr_dbd_t *handle)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>apr_dbd_prepare</name>
      <anchorfile>group___a_p_r___util___d_b_d.html</anchorfile>
      <anchor>ga19b6f9e8f3842b1b08f11916111cd99</anchor>
      <arglist>(const apr_dbd_driver_t *driver, apr_pool_t *pool, apr_dbd_t *handle, const char *query, const char *label, apr_dbd_prepared_t **statement)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>apr_dbd_pquery</name>
      <anchorfile>group___a_p_r___util___d_b_d.html</anchorfile>
      <anchor>g56138ae630391b1a013bd43358cdcf4b</anchor>
      <arglist>(const apr_dbd_driver_t *driver, apr_pool_t *pool, apr_dbd_t *handle, int *nrows, apr_dbd_prepared_t *statement, int nargs, const char **args)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>apr_dbd_pselect</name>
      <anchorfile>group___a_p_r___util___d_b_d.html</anchorfile>
      <anchor>g9244fc2997c87ef89358f46ab7a1abc0</anchor>
      <arglist>(const apr_dbd_driver_t *driver, apr_pool_t *pool, apr_dbd_t *handle, apr_dbd_results_t **res, apr_dbd_prepared_t *statement, int random, int nargs, const char **args)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>apr_dbd_pvquery</name>
      <anchorfile>group___a_p_r___util___d_b_d.html</anchorfile>
      <anchor>g95c0ac1458a94fa0161d82bb4dbe738d</anchor>
      <arglist>(const apr_dbd_driver_t *driver, apr_pool_t *pool, apr_dbd_t *handle, int *nrows, apr_dbd_prepared_t *statement,...)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>apr_dbd_pvselect</name>
      <anchorfile>group___a_p_r___util___d_b_d.html</anchorfile>
      <anchor>gf9a8af6b7ab148a59221b1ef6972970f</anchor>
      <arglist>(const apr_dbd_driver_t *driver, apr_pool_t *pool, apr_dbd_t *handle, apr_dbd_results_t **res, apr_dbd_prepared_t *statement, int random,...)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>apr_dbd_pbquery</name>
      <anchorfile>group___a_p_r___util___d_b_d.html</anchorfile>
      <anchor>g6d003ae40358f631aeb33da0ead90906</anchor>
      <arglist>(const apr_dbd_driver_t *driver, apr_pool_t *pool, apr_dbd_t *handle, int *nrows, apr_dbd_prepared_t *statement, const void **args)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>apr_dbd_pbselect</name>
      <anchorfile>group___a_p_r___util___d_b_d.html</anchorfile>
      <anchor>g1f00c276bd580c35579bc4e366939571</anchor>
      <arglist>(const apr_dbd_driver_t *driver, apr_pool_t *pool, apr_dbd_t *handle, apr_dbd_results_t **res, apr_dbd_prepared_t *statement, int random, const void **args)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>apr_dbd_pvbquery</name>
      <anchorfile>group___a_p_r___util___d_b_d.html</anchorfile>
      <anchor>g8f5ae6c8ad0c8c724fa31a41ac41c3cd</anchor>
      <arglist>(const apr_dbd_driver_t *driver, apr_pool_t *pool, apr_dbd_t *handle, int *nrows, apr_dbd_prepared_t *statement,...)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>apr_dbd_pvbselect</name>
      <anchorfile>group___a_p_r___util___d_b_d.html</anchorfile>
      <anchor>gb6f0cd2e0f63f0c27beb967979b39e41</anchor>
      <arglist>(const apr_dbd_driver_t *driver, apr_pool_t *pool, apr_dbd_t *handle, apr_dbd_results_t **res, apr_dbd_prepared_t *statement, int random,...)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_dbd_datum_get</name>
      <anchorfile>group___a_p_r___util___d_b_d.html</anchorfile>
      <anchor>gb98c880aba6f6bc932490a2b9baa5264</anchor>
      <arglist>(const apr_dbd_driver_t *driver, apr_dbd_row_t *row, int col, apr_dbd_type_e type, void *data)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>APR_Util_DBM</name>
    <title>DBM routines</title>
    <filename>group___a_p_r___util___d_b_m.html</filename>
    <subgroup>APR_Util_DBM_SDBM</subgroup>
    <class kind="struct">apr_datum_t</class>
    <member kind="define">
      <type>#define</type>
      <name>APR_DBM_READONLY</name>
      <anchorfile>group___a_p_r___util___d_b_m.html</anchorfile>
      <anchor>g12995c1e41e471974be9aec01247b742</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_DBM_READWRITE</name>
      <anchorfile>group___a_p_r___util___d_b_m.html</anchorfile>
      <anchor>gdfcec61581a0d0d8c726d3b253174b3a</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_DBM_RWCREATE</name>
      <anchorfile>group___a_p_r___util___d_b_m.html</anchorfile>
      <anchor>ga6ea90dc879edb56cb2325b45c49fd7c</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_DBM_RWTRUNC</name>
      <anchorfile>group___a_p_r___util___d_b_m.html</anchorfile>
      <anchor>g9390d754bc542cef506390d8e03f8c69</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>apr_dbm_t</type>
      <name>apr_dbm_t</name>
      <anchorfile>group___a_p_r___util___d_b_m.html</anchorfile>
      <anchor>g8cb9495772e83b6c9621742b78871ce7</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_dbm_open_ex</name>
      <anchorfile>group___a_p_r___util___d_b_m.html</anchorfile>
      <anchor>g50bec5831019310a0ad554cc8d5e0068</anchor>
      <arglist>(apr_dbm_t **dbm, const char *type, const char *name, apr_int32_t mode, apr_fileperms_t perm, apr_pool_t *cntxt)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_dbm_open</name>
      <anchorfile>group___a_p_r___util___d_b_m.html</anchorfile>
      <anchor>g6cbdd50fc93f881480b8a96ead202a03</anchor>
      <arglist>(apr_dbm_t **dbm, const char *name, apr_int32_t mode, apr_fileperms_t perm, apr_pool_t *cntxt)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>apr_dbm_close</name>
      <anchorfile>group___a_p_r___util___d_b_m.html</anchorfile>
      <anchor>g0e83207299fe586db289104137ac9dda</anchor>
      <arglist>(apr_dbm_t *dbm)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_dbm_fetch</name>
      <anchorfile>group___a_p_r___util___d_b_m.html</anchorfile>
      <anchor>gb99ae42a910d424ff143ddcc80345dcd</anchor>
      <arglist>(apr_dbm_t *dbm, apr_datum_t key, apr_datum_t *pvalue)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_dbm_store</name>
      <anchorfile>group___a_p_r___util___d_b_m.html</anchorfile>
      <anchor>gb05f022f2f4e3d0d18e35991c91bb6da</anchor>
      <arglist>(apr_dbm_t *dbm, apr_datum_t key, apr_datum_t value)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_dbm_delete</name>
      <anchorfile>group___a_p_r___util___d_b_m.html</anchorfile>
      <anchor>gbb7433ba5e429db24e6a6c9b2093b79e</anchor>
      <arglist>(apr_dbm_t *dbm, apr_datum_t key)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>apr_dbm_exists</name>
      <anchorfile>group___a_p_r___util___d_b_m.html</anchorfile>
      <anchor>g3e25adf0c35e4ea60beccc73a9cf8a91</anchor>
      <arglist>(apr_dbm_t *dbm, apr_datum_t key)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_dbm_firstkey</name>
      <anchorfile>group___a_p_r___util___d_b_m.html</anchorfile>
      <anchor>gbcc3791e85dc06a876a26e7735686613</anchor>
      <arglist>(apr_dbm_t *dbm, apr_datum_t *pkey)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_dbm_nextkey</name>
      <anchorfile>group___a_p_r___util___d_b_m.html</anchorfile>
      <anchor>g7391b506e6f857e04a4093aeda8d1c78</anchor>
      <arglist>(apr_dbm_t *dbm, apr_datum_t *pkey)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>apr_dbm_freedatum</name>
      <anchorfile>group___a_p_r___util___d_b_m.html</anchorfile>
      <anchor>gd98702224edbfc70a6a2877664a74b01</anchor>
      <arglist>(apr_dbm_t *dbm, apr_datum_t data)</arglist>
    </member>
    <member kind="function">
      <type>char *</type>
      <name>apr_dbm_geterror</name>
      <anchorfile>group___a_p_r___util___d_b_m.html</anchorfile>
      <anchor>g3a5f12fe099e57c12cd483ace5b1c130</anchor>
      <arglist>(apr_dbm_t *dbm, int *errcode, char *errbuf, apr_size_t errbufsize)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_dbm_get_usednames_ex</name>
      <anchorfile>group___a_p_r___util___d_b_m.html</anchorfile>
      <anchor>gf93eb50aca3b1b232c27875949145efb</anchor>
      <arglist>(apr_pool_t *pool, const char *type, const char *pathname, const char **used1, const char **used2)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>apr_dbm_get_usednames</name>
      <anchorfile>group___a_p_r___util___d_b_m.html</anchorfile>
      <anchor>g412a7f2a07cf91491d4d291790d9e4df</anchor>
      <arglist>(apr_pool_t *pool, const char *pathname, const char **used1, const char **used2)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>APR_Util_Hook</name>
    <title>Hook Functions</title>
    <filename>group___a_p_r___util___hook.html</filename>
    <subgroup>APR_Util_OPT_HOOK</subgroup>
    <member kind="define">
      <type>#define</type>
      <name>APR_IMPLEMENT_HOOK_GET_PROTO</name>
      <anchorfile>group___a_p_r___util___hook.html</anchorfile>
      <anchor>g320e483e7f860f3c555b4528d9abf41e</anchor>
      <arglist>(ns, link, name)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_DECLARE_EXTERNAL_HOOK</name>
      <anchorfile>group___a_p_r___util___hook.html</anchorfile>
      <anchor>g3f78b97be611af19de205ac15cf23af1</anchor>
      <arglist>(ns, link, ret, name, args)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_HOOK_STRUCT</name>
      <anchorfile>group___a_p_r___util___hook.html</anchorfile>
      <anchor>g0dcdfb814242c191ac36c34a4efbda22</anchor>
      <arglist>(members)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_HOOK_LINK</name>
      <anchorfile>group___a_p_r___util___hook.html</anchorfile>
      <anchor>gf525d29d6a0c04b6c62efe62104cbf01</anchor>
      <arglist>(name)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_IMPLEMENT_EXTERNAL_HOOK_BASE</name>
      <anchorfile>group___a_p_r___util___hook.html</anchorfile>
      <anchor>gc989f7fa3c8eca3f96b360c182cc688b</anchor>
      <arglist>(ns, link, name)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_IMPLEMENT_EXTERNAL_HOOK_VOID</name>
      <anchorfile>group___a_p_r___util___hook.html</anchorfile>
      <anchor>g778461267f1d8dab08003c851f0b8e4e</anchor>
      <arglist>(ns, link, name, args_decl, args_use)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_IMPLEMENT_EXTERNAL_HOOK_RUN_ALL</name>
      <anchorfile>group___a_p_r___util___hook.html</anchorfile>
      <anchor>gc3546697395695fe99e11205d7b0b4bd</anchor>
      <arglist>(ns, link, ret, name, args_decl, args_use, ok, decline)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_IMPLEMENT_EXTERNAL_HOOK_RUN_FIRST</name>
      <anchorfile>group___a_p_r___util___hook.html</anchorfile>
      <anchor>g92f3dd3071316e47a9320d0b3e9286a5</anchor>
      <arglist>(ns, link, ret, name, args_decl, args_use, decline)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_HOOK_REALLY_FIRST</name>
      <anchorfile>group___a_p_r___util___hook.html</anchorfile>
      <anchor>g4476db33781bbd5684d2d4041b944cac</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_HOOK_FIRST</name>
      <anchorfile>group___a_p_r___util___hook.html</anchorfile>
      <anchor>gf804b3cc837502c4628f0d8ebcb29734</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_HOOK_MIDDLE</name>
      <anchorfile>group___a_p_r___util___hook.html</anchorfile>
      <anchor>g46a34c28993e99673a4ac75084d30ecf</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_HOOK_LAST</name>
      <anchorfile>group___a_p_r___util___hook.html</anchorfile>
      <anchor>g1a9e60ca7fac7f975225b0da25c7495b</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_HOOK_REALLY_LAST</name>
      <anchorfile>group___a_p_r___util___hook.html</anchorfile>
      <anchor>g6bd3bff1e40180d4942f16998e619a92</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>apr_hook_sort_register</name>
      <anchorfile>group___a_p_r___util___hook.html</anchorfile>
      <anchor>g054fff20db06876d08484c9a08bb583a</anchor>
      <arglist>(const char *szHookName, apr_array_header_t **aHooks)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>apr_hook_sort_all</name>
      <anchorfile>group___a_p_r___util___hook.html</anchorfile>
      <anchor>g3e1d0f1ab2a7ea2c7d4939dbb470f362</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>apr_hook_debug_show</name>
      <anchorfile>group___a_p_r___util___hook.html</anchorfile>
      <anchor>gdc0bd91825cce24ff6d3f78ff886d5a7</anchor>
      <arglist>(const char *szName, const char *const *aszPre, const char *const *aszSucc)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>apr_hook_deregister_all</name>
      <anchorfile>group___a_p_r___util___hook.html</anchorfile>
      <anchor>g9a91bafc283bd426398e395a695b0e35</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="variable">
      <type>apr_pool_t *</type>
      <name>apr_hook_global_pool</name>
      <anchorfile>group___a_p_r___util___hook.html</anchorfile>
      <anchor>gea189125cb8f0a439fe49574b0b1174e</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int</type>
      <name>apr_hook_debug_enabled</name>
      <anchorfile>group___a_p_r___util___hook.html</anchorfile>
      <anchor>gb9d601e0d6db632340e054edd1c0e464</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char *</type>
      <name>apr_hook_debug_current</name>
      <anchorfile>group___a_p_r___util___hook.html</anchorfile>
      <anchor>gad3051246a587accb41cba7b9567fb98</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>APR_Util_LDAP</name>
    <title>LDAP</title>
    <filename>group___a_p_r___util___l_d_a_p.html</filename>
    <member kind="define">
      <type>#define</type>
      <name>APR_HAS_LDAP</name>
      <anchorfile>group___a_p_r___util___l_d_a_p.html</anchorfile>
      <anchor>g97ae9ae6ac09ee1d9837eed28afd764e</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_HAS_NETSCAPE_LDAPSDK</name>
      <anchorfile>group___a_p_r___util___l_d_a_p.html</anchorfile>
      <anchor>g46e2bf3cedfb64902534735b50846ce3</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_HAS_SOLARIS_LDAPSDK</name>
      <anchorfile>group___a_p_r___util___l_d_a_p.html</anchorfile>
      <anchor>g4d45b7c15daa1931c39c15ee4b448f65</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_HAS_NOVELL_LDAPSDK</name>
      <anchorfile>group___a_p_r___util___l_d_a_p.html</anchorfile>
      <anchor>g9aa49e25515d629a12ea959446733dcc</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_HAS_MOZILLA_LDAPSDK</name>
      <anchorfile>group___a_p_r___util___l_d_a_p.html</anchorfile>
      <anchor>g80299e03fe1a99180ec38a03272a377b</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_HAS_OPENLDAP_LDAPSDK</name>
      <anchorfile>group___a_p_r___util___l_d_a_p.html</anchorfile>
      <anchor>g84c39cde7657befcc27fc4cad3883cc2</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_HAS_MICROSOFT_LDAPSDK</name>
      <anchorfile>group___a_p_r___util___l_d_a_p.html</anchorfile>
      <anchor>gafd23814f14c16591776b6a8aef045b8</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_HAS_ZOS_LDAPSDK</name>
      <anchorfile>group___a_p_r___util___l_d_a_p.html</anchorfile>
      <anchor>g14eb849d0d7dca29924891f3aac9112f</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_HAS_OTHER_LDAPSDK</name>
      <anchorfile>group___a_p_r___util___l_d_a_p.html</anchorfile>
      <anchor>g1791ae13ab6f1c4a083eaf0d1102db99</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>APR_Util_MD4</name>
    <title>MD4 Library</title>
    <filename>group___a_p_r___util___m_d4.html</filename>
    <class kind="struct">apr_md4_ctx_t</class>
    <member kind="define">
      <type>#define</type>
      <name>APR_MD4_DIGESTSIZE</name>
      <anchorfile>group___a_p_r___util___m_d4.html</anchorfile>
      <anchor>gca5ee7150775441ac40f8fca83e8c702</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>apr_md4_ctx_t</type>
      <name>apr_md4_ctx_t</name>
      <anchorfile>group___a_p_r___util___m_d4.html</anchorfile>
      <anchor>g0043f24cc9a1874370c412eda84dcef5</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_md4_init</name>
      <anchorfile>group___a_p_r___util___m_d4.html</anchorfile>
      <anchor>g7498a7caddfa12d8746cba054af560ca</anchor>
      <arglist>(apr_md4_ctx_t *context)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_md4_set_xlate</name>
      <anchorfile>group___a_p_r___util___m_d4.html</anchorfile>
      <anchor>g52436829302c7709575112d76e8c4baf</anchor>
      <arglist>(apr_md4_ctx_t *context, apr_xlate_t *xlate)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_md4_update</name>
      <anchorfile>group___a_p_r___util___m_d4.html</anchorfile>
      <anchor>g3ef01f727f939376645f764d6cbf2117</anchor>
      <arglist>(apr_md4_ctx_t *context, const unsigned char *input, apr_size_t inputLen)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_md4_final</name>
      <anchorfile>group___a_p_r___util___m_d4.html</anchorfile>
      <anchor>g3588dc327a19a69a4c0e8be19cf5e7ba</anchor>
      <arglist>(unsigned char digest[APR_MD4_DIGESTSIZE], apr_md4_ctx_t *context)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_md4</name>
      <anchorfile>group___a_p_r___util___m_d4.html</anchorfile>
      <anchor>gad5541e42634ddd63a21bedf1f3c6bf0</anchor>
      <arglist>(unsigned char digest[APR_MD4_DIGESTSIZE], const unsigned char *input, apr_size_t inputLen)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>APR_MD5</name>
    <title>MD5 Routines</title>
    <filename>group___a_p_r___m_d5.html</filename>
    <class kind="struct">apr_md5_ctx_t</class>
    <member kind="define">
      <type>#define</type>
      <name>APR_MD5_DIGESTSIZE</name>
      <anchorfile>group___a_p_r___m_d5.html</anchorfile>
      <anchor>gec0d977a03646cb01a9ce4f156ad7049</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>apr_md5_ctx_t</type>
      <name>apr_md5_ctx_t</name>
      <anchorfile>group___a_p_r___m_d5.html</anchorfile>
      <anchor>gaba81a095b07466618f299b2d3d9b930</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_md5_init</name>
      <anchorfile>group___a_p_r___m_d5.html</anchorfile>
      <anchor>g8ab27e962e5ad4e01e74175bcfb52af8</anchor>
      <arglist>(apr_md5_ctx_t *context)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_md5_set_xlate</name>
      <anchorfile>group___a_p_r___m_d5.html</anchorfile>
      <anchor>g4a2f678278b9dad177259ce183da013b</anchor>
      <arglist>(apr_md5_ctx_t *context, apr_xlate_t *xlate)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_md5_update</name>
      <anchorfile>group___a_p_r___m_d5.html</anchorfile>
      <anchor>g2a73efa47b69ca260a41c851f39aa8e1</anchor>
      <arglist>(apr_md5_ctx_t *context, const void *input, apr_size_t inputLen)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_md5_final</name>
      <anchorfile>group___a_p_r___m_d5.html</anchorfile>
      <anchor>gec52db64037a58ed6030a12f1060610c</anchor>
      <arglist>(unsigned char digest[APR_MD5_DIGESTSIZE], apr_md5_ctx_t *context)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_md5</name>
      <anchorfile>group___a_p_r___m_d5.html</anchorfile>
      <anchor>g74079aa961c72d157508a2f31c18e289</anchor>
      <arglist>(unsigned char digest[APR_MD5_DIGESTSIZE], const void *input, apr_size_t inputLen)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_md5_encode</name>
      <anchorfile>group___a_p_r___m_d5.html</anchorfile>
      <anchor>g8e38aa9aede424191901b22fe34081f3</anchor>
      <arglist>(const char *password, const char *salt, char *result, apr_size_t nbytes)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_password_validate</name>
      <anchorfile>group___a_p_r___m_d5.html</anchorfile>
      <anchor>g5ddcb2395541250198fb7690c5d1fe0c</anchor>
      <arglist>(const char *passwd, const char *hash)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>APR_Util_MC</name>
    <title>Memcached Client Routines</title>
    <filename>group___a_p_r___util___m_c.html</filename>
    <class kind="struct">apr_memcache_server_t</class>
    <class kind="struct">apr_memcache_t</class>
    <class kind="struct">apr_memcache_value_t</class>
    <class kind="struct">apr_memcache_stats_t</class>
    <member kind="typedef">
      <type>apr_memcache_conn_t</type>
      <name>apr_memcache_conn_t</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>g9801000a269bfcff1ec39883f6d5d430</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>apr_memcache_server_t</type>
      <name>apr_memcache_server_t</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>gb135feeb15f3c31912b1e2795c8b7a4e</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <name>apr_memcache_server_status_t</name>
      <anchor>g7ddcdc3cff627563bbfca0d73cfe7a9f</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>APR_MC_SERVER_LIVE</name>
      <anchor>gg7ddcdc3cff627563bbfca0d73cfe7a9f19a18372aa0391203b9bd8eaa69f5cf2</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>APR_MC_SERVER_DEAD</name>
      <anchor>gg7ddcdc3cff627563bbfca0d73cfe7a9ff519062f8ee66c71ba1d703d0238edd5</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>APR_DECLARE</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>g852eaa5d78790ad83f9d2d3bdff81fcd</anchor>
      <arglist>(apr_uint32_t) apr_memcache_hash(const char *data</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>APR_DECLARE</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>g7f90210372c21721a45bc901b43601a4</anchor>
      <arglist>(apr_memcache_server_t *) apr_memcache_find_server_hash(apr_memcache_t *mc</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>APR_DECLARE</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>gb046f205cb0c9a3bb104fac8d26fb6cd</anchor>
      <arglist>(apr_status_t) apr_memcache_add_server(apr_memcache_t *mc</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>apr_memcache_add_multget_key</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>gebf4cda65413b926a202b8b7ed847930</anchor>
      <arglist>(apr_pool_t *data_pool, const char *key, apr_hash_t **values)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>apr_memcache_multgetp</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>gefe2f1487afe328403ccdae0e0024a6d</anchor>
      <arglist>(apr_memcache_t *mc, apr_pool_t *temp_pool, apr_pool_t *data_pool, apr_hash_t *values)</arglist>
    </member>
    <member kind="variable">
      <type>apr_size_t</type>
      <name>data_len</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>g5358156d49dab554db5ab905b15d0216</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const apr_uint32_t</type>
      <name>hash</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>g3b48afab70c4776ff981743c156e4c94</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apr_memcache_server_t *</type>
      <name>server</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>g70e14b5d9c89465e7d745e4c3f0a36f4</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char *</type>
      <name>host</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>ge299ba32612accbee735da724e29962a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char apr_port_t</type>
      <name>port</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>g2eb856cbbcc7de452bf099efaea5b36f</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apr_memcache_server_t *</type>
      <name>ms</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>gc0247808e1316fd01e8891b954053329</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apr_memcache_server_t *</type>
      <name>ms</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>gc0247808e1316fd01e8891b954053329</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char *</type>
      <name>host</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>ge299ba32612accbee735da724e29962a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char apr_port_t</type>
      <name>port</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>g2eb856cbbcc7de452bf099efaea5b36f</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char apr_port_t apr_uint32_t</type>
      <name>min</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>g12f1897ba538c47db8c5f62a206fb023</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char apr_port_t apr_uint32_t apr_uint32_t</type>
      <name>smax</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>gecdf08c5baa49e361b7ada8f5886847d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char apr_port_t apr_uint32_t apr_uint32_t apr_uint32_t</type>
      <name>max</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>g94e0986601932ae07e98004db4d56cf5</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char apr_port_t apr_uint32_t apr_uint32_t apr_uint32_t apr_uint32_t</type>
      <name>ttl</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>g8ddb5d824da3fe8793127f907c128289</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char apr_port_t apr_uint32_t apr_uint32_t apr_uint32_t apr_uint32_t apr_memcache_server_t **</type>
      <name>ns</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>g6a8b65c265c195b5bed28d892bba0942</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apr_uint16_t</type>
      <name>max_servers</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>gca02fb6d2ec6a0407b0e908d9eb30716</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apr_uint16_t apr_uint32_t</type>
      <name>flags</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>g260cfc81028e9876c5ed22308d03d908</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apr_uint16_t apr_uint32_t apr_memcache_t **</type>
      <name>mc</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>g32d07d9e6f57c586d9e7c1af38fe4947</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apr_pool_t *</type>
      <name>p</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>g1694c2a3936b5c65143864201d1e044a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apr_pool_t const char *</type>
      <name>key</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>g842ed24c713e30e6439c3720f05938b4</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apr_pool_t const char char **</type>
      <name>baton</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>g754f4a6138669d883ec4f53a316d3709</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apr_pool_t const char char apr_size_t *</type>
      <name>len</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>g7f0494f6390dda2989c702055127817c</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apr_pool_t const char char apr_size_t apr_uint16_t *</type>
      <name>flags</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>g34a8d49502a3a93e20fdae751cb24736</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char *</type>
      <name>key</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>g41c836d1cb3f7a0911a475bc7cf90078</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char char *</type>
      <name>baton</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>g656e5ea9f493d8fd6da0cdb3a49e4b4f</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char char const apr_size_t</type>
      <name>data_size</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>g644a4eb6ef4dc02779704ae36beecade</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char char const apr_size_t apr_uint32_t</type>
      <name>timeout</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>ga999c7ab703603bd5db7a7af6ac259e6</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char char const apr_size_t apr_uint32_t apr_uint16_t</type>
      <name>flags</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>g4662477274ca279e1c54e8b56e685549</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char *</type>
      <name>key</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>g41c836d1cb3f7a0911a475bc7cf90078</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char char *</type>
      <name>baton</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>g656e5ea9f493d8fd6da0cdb3a49e4b4f</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char char const apr_size_t</type>
      <name>data_size</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>g644a4eb6ef4dc02779704ae36beecade</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char char const apr_size_t apr_uint32_t</type>
      <name>timeout</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>ga999c7ab703603bd5db7a7af6ac259e6</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char char const apr_size_t apr_uint32_t apr_uint16_t</type>
      <name>flags</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>g4662477274ca279e1c54e8b56e685549</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char *</type>
      <name>key</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>g41c836d1cb3f7a0911a475bc7cf90078</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char char *</type>
      <name>data</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>gb0b2c3a8aeda505fd8b6253944e3a3d0</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char char const apr_size_t</type>
      <name>data_size</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>g644a4eb6ef4dc02779704ae36beecade</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char char const apr_size_t apr_uint32_t</type>
      <name>timeout</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>ga999c7ab703603bd5db7a7af6ac259e6</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char char const apr_size_t apr_uint32_t apr_uint16_t</type>
      <name>flags</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>g4662477274ca279e1c54e8b56e685549</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char *</type>
      <name>key</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>g41c836d1cb3f7a0911a475bc7cf90078</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char apr_uint32_t</type>
      <name>timeout</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>g6934dc8c7865171fa00221483a5c9d0d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char *</type>
      <name>key</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>g41c836d1cb3f7a0911a475bc7cf90078</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char apr_int32_t</type>
      <name>n</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>g280d09d604193c789b9f4741661de31b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char apr_int32_t apr_uint32_t *</type>
      <name>nv</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>g86dfdcfc2e078ca6f3e00170718ac905</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char *</type>
      <name>key</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>g41c836d1cb3f7a0911a475bc7cf90078</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char apr_int32_t</type>
      <name>n</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>g280d09d604193c789b9f4741661de31b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char apr_int32_t apr_uint32_t *</type>
      <name>new_value</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>gc56fb8cc07b8d62081c313eadf90a573</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apr_pool_t *</type>
      <name>p</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>g1694c2a3936b5c65143864201d1e044a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apr_pool_t char **</type>
      <name>baton</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>g1502f08f57b6b78fbb69423d0149bc18</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apr_pool_t *</type>
      <name>p</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>g1694c2a3936b5c65143864201d1e044a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apr_pool_t apr_memcache_stats_t **</type>
      <name>stats</name>
      <anchorfile>group___a_p_r___util___m_c.html</anchorfile>
      <anchor>gba635ba268417079099ea6a7c1513047</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>APR_Util_Opt</name>
    <title>Optional Functions</title>
    <filename>group___a_p_r___util___opt.html</filename>
    <member kind="define">
      <type>#define</type>
      <name>APR_OPTIONAL_FN_TYPE</name>
      <anchorfile>group___a_p_r___util___opt.html</anchorfile>
      <anchor>gd16be52fdc9672d4cc2a689eb79cfe1b</anchor>
      <arglist>(name)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_DECLARE_OPTIONAL_FN</name>
      <anchorfile>group___a_p_r___util___opt.html</anchorfile>
      <anchor>gc7fd10ae0fc745fe48ee90345f0b730d</anchor>
      <arglist>(ret, name, args)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_REGISTER_OPTIONAL_FN</name>
      <anchorfile>group___a_p_r___util___opt.html</anchorfile>
      <anchor>g8a3f199b6460283af6164336c926be43</anchor>
      <arglist>(name)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_RETRIEVE_OPTIONAL_FN</name>
      <anchorfile>group___a_p_r___util___opt.html</anchorfile>
      <anchor>gd8c2989e7e89245137bc84a709443925</anchor>
      <arglist>(name)</arglist>
    </member>
    <member kind="typedef">
      <type>void(</type>
      <name>apr_opt_fn_t</name>
      <anchorfile>group___a_p_r___util___opt.html</anchorfile>
      <anchor>g98bef8c1c54c627088fba31dd8c6dd49</anchor>
      <arglist>)(void)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>apr_dynamic_fn_register</name>
      <anchorfile>group___a_p_r___util___opt.html</anchorfile>
      <anchor>gbd053d1d62e25b35c7214f280227dcc0</anchor>
      <arglist>(const char *szName, apr_opt_fn_t *pfn)</arglist>
    </member>
    <member kind="function">
      <type>apr_opt_fn_t *</type>
      <name>apr_dynamic_fn_retrieve</name>
      <anchorfile>group___a_p_r___util___opt.html</anchorfile>
      <anchor>gdcf7d8c80e8e2b7e2cb36dafa1620579</anchor>
      <arglist>(const char *szName)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>APR_Util_OPT_HOOK</name>
    <title>Optional Hook Functions</title>
    <filename>group___a_p_r___util___o_p_t___h_o_o_k.html</filename>
    <member kind="define">
      <type>#define</type>
      <name>APR_OPTIONAL_HOOK</name>
      <anchorfile>group___a_p_r___util___o_p_t___h_o_o_k.html</anchorfile>
      <anchor>g79437ac440ebae7cb2b3122ad6393f1a</anchor>
      <arglist>(ns, name, pfn, aszPre, aszSucc, nOrder)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_IMPLEMENT_OPTIONAL_HOOK_RUN_ALL</name>
      <anchorfile>group___a_p_r___util___o_p_t___h_o_o_k.html</anchorfile>
      <anchor>g5197eadc0461880e47687522da203afb</anchor>
      <arglist>(ns, link, ret, name, args_decl, args_use, ok, decline)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>apr_optional_hook_add</name>
      <anchorfile>group___a_p_r___util___o_p_t___h_o_o_k.html</anchorfile>
      <anchor>g7562a06f6d87425d7729a8dcdc3cc7fa</anchor>
      <arglist>(const char *szName, void(*pfn)(void), const char *const *aszPre, const char *const *aszSucc, int nOrder)</arglist>
    </member>
    <member kind="function">
      <type>apr_array_header_t *</type>
      <name>apr_optional_hook_get</name>
      <anchorfile>group___a_p_r___util___o_p_t___h_o_o_k.html</anchorfile>
      <anchor>gfbce21247691f1d8d785112071f870f8</anchor>
      <arglist>(const char *szName)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>APR_Util_FIFO</name>
    <title>Thread Safe FIFO bounded queue</title>
    <filename>group___a_p_r___util___f_i_f_o.html</filename>
    <member kind="typedef">
      <type>apr_queue_t</type>
      <name>apr_queue_t</name>
      <anchorfile>group___a_p_r___util___f_i_f_o.html</anchorfile>
      <anchor>g4fb6e57b49285fa45e4f598dbcd7d014</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_queue_create</name>
      <anchorfile>group___a_p_r___util___f_i_f_o.html</anchorfile>
      <anchor>g6c57799d9d0500dce636a537c743756f</anchor>
      <arglist>(apr_queue_t **queue, unsigned int queue_capacity, apr_pool_t *a)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_queue_push</name>
      <anchorfile>group___a_p_r___util___f_i_f_o.html</anchorfile>
      <anchor>g2606bc06b90c1e5b76abf55a3b2df722</anchor>
      <arglist>(apr_queue_t *queue, void *data)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_queue_pop</name>
      <anchorfile>group___a_p_r___util___f_i_f_o.html</anchorfile>
      <anchor>g65697cae0a71b83dd5b4c3e8f1735cc8</anchor>
      <arglist>(apr_queue_t *queue, void **data)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_queue_trypush</name>
      <anchorfile>group___a_p_r___util___f_i_f_o.html</anchorfile>
      <anchor>g290c91d5d42ac275e215d379ddba7722</anchor>
      <arglist>(apr_queue_t *queue, void *data)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_queue_trypop</name>
      <anchorfile>group___a_p_r___util___f_i_f_o.html</anchorfile>
      <anchor>g5642e67dbee42e1334924895b66d2192</anchor>
      <arglist>(apr_queue_t *queue, void **data)</arglist>
    </member>
    <member kind="function">
      <type>unsigned int</type>
      <name>apr_queue_size</name>
      <anchorfile>group___a_p_r___util___f_i_f_o.html</anchorfile>
      <anchor>g2c1ce13a013bfcc3b6ec5d7de37846c6</anchor>
      <arglist>(apr_queue_t *queue)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_queue_interrupt_all</name>
      <anchorfile>group___a_p_r___util___f_i_f_o.html</anchorfile>
      <anchor>g998296e3a87fbcf94928887696032a3c</anchor>
      <arglist>(apr_queue_t *queue)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_queue_term</name>
      <anchorfile>group___a_p_r___util___f_i_f_o.html</anchorfile>
      <anchor>g9a342118932c8918fa9e4307622e0617</anchor>
      <arglist>(apr_queue_t *queue)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>APR_Util_RL</name>
    <title>Resource List Routines</title>
    <filename>group___a_p_r___util___r_l.html</filename>
    <member kind="typedef">
      <type>apr_reslist_t</type>
      <name>apr_reslist_t</name>
      <anchorfile>group___a_p_r___util___r_l.html</anchorfile>
      <anchor>g2f25985a4666e98aacf0e1b8e3427f4c</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>apr_status_t(*)</type>
      <name>apr_reslist_constructor</name>
      <anchorfile>group___a_p_r___util___r_l.html</anchorfile>
      <anchor>g6e9eef7e4d2a8a576d2a71782a8a9b7b</anchor>
      <arglist>(void **resource, void *params, apr_pool_t *pool)</arglist>
    </member>
    <member kind="typedef">
      <type>apr_status_t(*)</type>
      <name>apr_reslist_destructor</name>
      <anchorfile>group___a_p_r___util___r_l.html</anchorfile>
      <anchor>g8b1565606af2d5bb91d8a65e010fb30c</anchor>
      <arglist>(void *resource, void *params, apr_pool_t *pool)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_reslist_create</name>
      <anchorfile>group___a_p_r___util___r_l.html</anchorfile>
      <anchor>g154edff9ed0928e2290aa81cd9ab56df</anchor>
      <arglist>(apr_reslist_t **reslist, int min, int smax, int hmax, apr_interval_time_t ttl, apr_reslist_constructor con, apr_reslist_destructor de, void *params, apr_pool_t *pool)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_reslist_destroy</name>
      <anchorfile>group___a_p_r___util___r_l.html</anchorfile>
      <anchor>g31829a18fcc0308624f07c2fd86467ac</anchor>
      <arglist>(apr_reslist_t *reslist)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_reslist_acquire</name>
      <anchorfile>group___a_p_r___util___r_l.html</anchorfile>
      <anchor>gc64594efd763d6bca2755c96e9341f8a</anchor>
      <arglist>(apr_reslist_t *reslist, void **resource)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_reslist_release</name>
      <anchorfile>group___a_p_r___util___r_l.html</anchorfile>
      <anchor>gbbd325ed5d2b0ae14fc914dd7da11ab3</anchor>
      <arglist>(apr_reslist_t *reslist, void *resource)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>apr_reslist_timeout_set</name>
      <anchorfile>group___a_p_r___util___r_l.html</anchorfile>
      <anchor>g0c7319e812f0322049aedc8bdf8675e7</anchor>
      <arglist>(apr_reslist_t *reslist, apr_interval_time_t timeout)</arglist>
    </member>
    <member kind="function">
      <type>apr_uint32_t</type>
      <name>apr_reslist_acquired_count</name>
      <anchorfile>group___a_p_r___util___r_l.html</anchorfile>
      <anchor>ge0a729e7e0bcd7badfcd57508c9de5fa</anchor>
      <arglist>(apr_reslist_t *reslist)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_reslist_invalidate</name>
      <anchorfile>group___a_p_r___util___r_l.html</anchorfile>
      <anchor>gf9bd72dad6a757187ccd3e5593d9a331</anchor>
      <arglist>(apr_reslist_t *reslist, void *resource)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>APR_Util_RMM</name>
    <title>Relocatable Memory Management Routines</title>
    <filename>group___a_p_r___util___r_m_m.html</filename>
    <member kind="typedef">
      <type>apr_rmm_t</type>
      <name>apr_rmm_t</name>
      <anchorfile>group___a_p_r___util___r_m_m.html</anchorfile>
      <anchor>ge5eff8ce4f9b6c314bcb66da5ddee373</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>apr_size_t</type>
      <name>apr_rmm_off_t</name>
      <anchorfile>group___a_p_r___util___r_m_m.html</anchorfile>
      <anchor>g21f622eeacfc8032a9a60f9f619b8776</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_rmm_init</name>
      <anchorfile>group___a_p_r___util___r_m_m.html</anchorfile>
      <anchor>gac95f46ea30d105da794997c244f967c</anchor>
      <arglist>(apr_rmm_t **rmm, apr_anylock_t *lock, void *membuf, apr_size_t memsize, apr_pool_t *cont)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_rmm_destroy</name>
      <anchorfile>group___a_p_r___util___r_m_m.html</anchorfile>
      <anchor>g146a63d6b0283dbb8a73770a41d08fa8</anchor>
      <arglist>(apr_rmm_t *rmm)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_rmm_attach</name>
      <anchorfile>group___a_p_r___util___r_m_m.html</anchorfile>
      <anchor>ga78a320836add3af5361e1091152d091</anchor>
      <arglist>(apr_rmm_t **rmm, apr_anylock_t *lock, void *membuf, apr_pool_t *cont)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_rmm_detach</name>
      <anchorfile>group___a_p_r___util___r_m_m.html</anchorfile>
      <anchor>g3c38be767c654be8f53771ba4356498a</anchor>
      <arglist>(apr_rmm_t *rmm)</arglist>
    </member>
    <member kind="function">
      <type>apr_rmm_off_t</type>
      <name>apr_rmm_malloc</name>
      <anchorfile>group___a_p_r___util___r_m_m.html</anchorfile>
      <anchor>ga9a31df54bcc4c0af984c9bc2461827a</anchor>
      <arglist>(apr_rmm_t *rmm, apr_size_t reqsize)</arglist>
    </member>
    <member kind="function">
      <type>apr_rmm_off_t</type>
      <name>apr_rmm_realloc</name>
      <anchorfile>group___a_p_r___util___r_m_m.html</anchorfile>
      <anchor>g19bfda917948efe8115ae7f9f2a973f1</anchor>
      <arglist>(apr_rmm_t *rmm, void *entity, apr_size_t reqsize)</arglist>
    </member>
    <member kind="function">
      <type>apr_rmm_off_t</type>
      <name>apr_rmm_calloc</name>
      <anchorfile>group___a_p_r___util___r_m_m.html</anchorfile>
      <anchor>gc0036240d63edc38971f71b94c393b82</anchor>
      <arglist>(apr_rmm_t *rmm, apr_size_t reqsize)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_rmm_free</name>
      <anchorfile>group___a_p_r___util___r_m_m.html</anchorfile>
      <anchor>g35a889a6caf6002f5fd0c49990ab5e17</anchor>
      <arglist>(apr_rmm_t *rmm, apr_rmm_off_t entity)</arglist>
    </member>
    <member kind="function">
      <type>void *</type>
      <name>apr_rmm_addr_get</name>
      <anchorfile>group___a_p_r___util___r_m_m.html</anchorfile>
      <anchor>g88c36ee0ad4abdc643e01971c18fbdf8</anchor>
      <arglist>(apr_rmm_t *rmm, apr_rmm_off_t entity)</arglist>
    </member>
    <member kind="function">
      <type>apr_rmm_off_t</type>
      <name>apr_rmm_offset_get</name>
      <anchorfile>group___a_p_r___util___r_m_m.html</anchorfile>
      <anchor>g40cf37655a031012c9325be3cc278683</anchor>
      <arglist>(apr_rmm_t *rmm, void *entity)</arglist>
    </member>
    <member kind="function">
      <type>apr_size_t</type>
      <name>apr_rmm_overhead_get</name>
      <anchorfile>group___a_p_r___util___r_m_m.html</anchorfile>
      <anchor>g8fc0b723aa71eff2f81c9d05212c200c</anchor>
      <arglist>(int n)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>APR_Util_DBM_SDBM</name>
    <title>SDBM library</title>
    <filename>group___a_p_r___util___d_b_m___s_d_b_m.html</filename>
    <class kind="struct">apr_sdbm_datum_t</class>
    <member kind="define">
      <type>#define</type>
      <name>APR_SDBM_DIRFEXT</name>
      <anchorfile>group___a_p_r___util___d_b_m___s_d_b_m.html</anchorfile>
      <anchor>g0ff77f72468e824aad6e71c276fc4c73</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_SDBM_PAGFEXT</name>
      <anchorfile>group___a_p_r___util___d_b_m___s_d_b_m.html</anchorfile>
      <anchor>g48c662a7711370df6904848abce51d08</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_SDBM_INSERT</name>
      <anchorfile>group___a_p_r___util___d_b_m___s_d_b_m.html</anchorfile>
      <anchor>gf78d4b4d7ab410d18a12147aa67dc4f1</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_SDBM_REPLACE</name>
      <anchorfile>group___a_p_r___util___d_b_m___s_d_b_m.html</anchorfile>
      <anchor>g533f72464d7c6c1a79cf22c6c06f3fcf</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_SDBM_INSERTDUP</name>
      <anchorfile>group___a_p_r___util___d_b_m___s_d_b_m.html</anchorfile>
      <anchor>g23e98a293c9585a0dcdd8f5127e617c9</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>apr_sdbm_t</type>
      <name>apr_sdbm_t</name>
      <anchorfile>group___a_p_r___util___d_b_m___s_d_b_m.html</anchorfile>
      <anchor>gb1cd57a5984a7f350cd83705b6cc308a</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_sdbm_open</name>
      <anchorfile>group___a_p_r___util___d_b_m___s_d_b_m.html</anchorfile>
      <anchor>g5ede50dca933e162629cc43245036e41</anchor>
      <arglist>(apr_sdbm_t **db, const char *name, apr_int32_t mode, apr_fileperms_t perms, apr_pool_t *p)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_sdbm_close</name>
      <anchorfile>group___a_p_r___util___d_b_m___s_d_b_m.html</anchorfile>
      <anchor>gfa0176fc27d32191b8cfaed762301a95</anchor>
      <arglist>(apr_sdbm_t *db)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_sdbm_lock</name>
      <anchorfile>group___a_p_r___util___d_b_m___s_d_b_m.html</anchorfile>
      <anchor>g42bf926b5dacb9c755d3b7bace9c7492</anchor>
      <arglist>(apr_sdbm_t *db, int type)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_sdbm_unlock</name>
      <anchorfile>group___a_p_r___util___d_b_m___s_d_b_m.html</anchorfile>
      <anchor>g86480c8d22e51a35677f310ad81ee38d</anchor>
      <arglist>(apr_sdbm_t *db)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_sdbm_fetch</name>
      <anchorfile>group___a_p_r___util___d_b_m___s_d_b_m.html</anchorfile>
      <anchor>g92819c791734392bfaf0e5564a20ae18</anchor>
      <arglist>(apr_sdbm_t *db, apr_sdbm_datum_t *value, apr_sdbm_datum_t key)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_sdbm_store</name>
      <anchorfile>group___a_p_r___util___d_b_m___s_d_b_m.html</anchorfile>
      <anchor>ge6ba8345c1abe9f757841665b9daa866</anchor>
      <arglist>(apr_sdbm_t *db, apr_sdbm_datum_t key, apr_sdbm_datum_t value, int opt)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_sdbm_delete</name>
      <anchorfile>group___a_p_r___util___d_b_m___s_d_b_m.html</anchorfile>
      <anchor>g3de38cbc4c06d4fd9a5f3e08d0109af0</anchor>
      <arglist>(apr_sdbm_t *db, const apr_sdbm_datum_t key)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_sdbm_firstkey</name>
      <anchorfile>group___a_p_r___util___d_b_m___s_d_b_m.html</anchorfile>
      <anchor>g8f5c66960bcd43c75e271f757c110ff5</anchor>
      <arglist>(apr_sdbm_t *db, apr_sdbm_datum_t *key)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_sdbm_nextkey</name>
      <anchorfile>group___a_p_r___util___d_b_m___s_d_b_m.html</anchorfile>
      <anchor>g82eb237fc09febc743b14abb6a59fa45</anchor>
      <arglist>(apr_sdbm_t *db, apr_sdbm_datum_t *key)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>apr_sdbm_rdonly</name>
      <anchorfile>group___a_p_r___util___d_b_m___s_d_b_m.html</anchorfile>
      <anchor>g93b350d3aab8a8c41934a89da8a4351a</anchor>
      <arglist>(apr_sdbm_t *db)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>APR_Util_SSL</name>
    <title>SSL socket routines</title>
    <filename>group___a_p_r___util___s_s_l.html</filename>
    <member kind="typedef">
      <type>apr_ssl_factory</type>
      <name>apr_ssl_factory_t</name>
      <anchorfile>group___a_p_r___util___s_s_l.html</anchorfile>
      <anchor>gf45b4f4d0e8641392b25a9a505fe32e5</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>apr_ssl_socket</type>
      <name>apr_ssl_socket_t</name>
      <anchorfile>group___a_p_r___util___s_s_l.html</anchorfile>
      <anchor>g6732fe4958ad4a806ebd67b6de977176</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <name>apr_ssl_factory_type_e</name>
      <anchor>gd15e84ccd1d466840b71796fcf0e3dc9</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>APR_SSL_FACTORY_SERVER</name>
      <anchor>ggd15e84ccd1d466840b71796fcf0e3dc9f5b1e327a59a76868ccbf82f716230c8</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>APR_SSL_FACTORY_CLIENT</name>
      <anchor>ggd15e84ccd1d466840b71796fcf0e3dc93f22ef4b88ac3b1f8f8233d5c2e0ab54</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_ssl_factory_create</name>
      <anchorfile>group___a_p_r___util___s_s_l.html</anchorfile>
      <anchor>gefa85ebc22943c5f640cbd9514123ab9</anchor>
      <arglist>(apr_ssl_factory_t **, const char *, const char *, const char *, apr_ssl_factory_type_e, apr_pool_t *)</arglist>
    </member>
    <member kind="function">
      <type>const char *</type>
      <name>apr_ssl_library_name</name>
      <anchorfile>group___a_p_r___util___s_s_l.html</anchorfile>
      <anchor>gdf416172b3404bbbfd5fedcff31d1df6</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_ssl_socket_create</name>
      <anchorfile>group___a_p_r___util___s_s_l.html</anchorfile>
      <anchor>gf938ef9c97a5165bd8fcbd7b98736883</anchor>
      <arglist>(apr_ssl_socket_t **, int, int, int, apr_ssl_factory_t *, apr_pool_t *)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_ssl_socket_close</name>
      <anchorfile>group___a_p_r___util___s_s_l.html</anchorfile>
      <anchor>g8e0438971c5927ba79e078b73122917f</anchor>
      <arglist>(apr_ssl_socket_t *)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_ssl_socket_connect</name>
      <anchorfile>group___a_p_r___util___s_s_l.html</anchorfile>
      <anchor>g8baa1fa5149e8a3496f472e6469be8ad</anchor>
      <arglist>(apr_ssl_socket_t *, apr_sockaddr_t *)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_ssl_socket_send</name>
      <anchorfile>group___a_p_r___util___s_s_l.html</anchorfile>
      <anchor>g314e2920ee8d37e71ca452cb333b09df</anchor>
      <arglist>(apr_ssl_socket_t *, const char *, apr_size_t *)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_ssl_socket_recv</name>
      <anchorfile>group___a_p_r___util___s_s_l.html</anchorfile>
      <anchor>g5858e20f4919b9a242df32204dd1dc2f</anchor>
      <arglist>(apr_ssl_socket_t *, char *, apr_size_t *)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_ssl_socket_bind</name>
      <anchorfile>group___a_p_r___util___s_s_l.html</anchorfile>
      <anchor>gd48d442ad6dd57c5f56e68e177a98bac</anchor>
      <arglist>(apr_ssl_socket_t *, apr_sockaddr_t *)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_ssl_socket_listen</name>
      <anchorfile>group___a_p_r___util___s_s_l.html</anchorfile>
      <anchor>g12afb47b7668498c65d23a463aa30dd2</anchor>
      <arglist>(apr_ssl_socket_t *, apr_int32_t)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_ssl_socket_accept</name>
      <anchorfile>group___a_p_r___util___s_s_l.html</anchorfile>
      <anchor>ge4016cf9eb6fcba89be5f483f5cff333</anchor>
      <arglist>(apr_ssl_socket_t **, apr_ssl_socket_t *, apr_pool_t *)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_ssl_socket_raw_error</name>
      <anchorfile>group___a_p_r___util___s_s_l.html</anchorfile>
      <anchor>g1e1c9a97cded5c59dbd83b779b2d67f6</anchor>
      <arglist>(apr_ssl_socket_t *)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_pollset_add_ssl_socket</name>
      <anchorfile>group___a_p_r___util___s_s_l.html</anchorfile>
      <anchor>ge58304b5d70cbc0f7cdc3586763ccf29</anchor>
      <arglist>(apr_pollset_t *, apr_ssl_socket_t *)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_pollset_remove_ssl_socket</name>
      <anchorfile>group___a_p_r___util___s_s_l.html</anchorfile>
      <anchor>gef87870c16fdf8755bcdacea12ac277b</anchor>
      <arglist>(apr_ssl_socket_t *)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_ssl_socket_set_poll_events</name>
      <anchorfile>group___a_p_r___util___s_s_l.html</anchorfile>
      <anchor>g27399f8608d974c1662986c146d37136</anchor>
      <arglist>(apr_ssl_socket_t *, apr_int16_t)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>APR_Util_StrMatch</name>
    <title>String matching routines</title>
    <filename>group___a_p_r___util___str_match.html</filename>
    <class kind="struct">apr_strmatch_pattern</class>
    <member kind="typedef">
      <type>apr_strmatch_pattern</type>
      <name>apr_strmatch_pattern</name>
      <anchorfile>group___a_p_r___util___str_match.html</anchorfile>
      <anchor>g1ebe05292f73000cf6973910d5e6c189</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>const char *</type>
      <name>apr_strmatch</name>
      <anchorfile>group___a_p_r___util___str_match.html</anchorfile>
      <anchor>g3481aa892c05ccc64ab9d5ab60f83328</anchor>
      <arglist>(const apr_strmatch_pattern *pattern, const char *s, apr_size_t slen)</arglist>
    </member>
    <member kind="function">
      <type>const apr_strmatch_pattern *</type>
      <name>apr_strmatch_precompile</name>
      <anchorfile>group___a_p_r___util___str_match.html</anchorfile>
      <anchor>g0745661741f86cb1d106d485cb0bf9ba</anchor>
      <arglist>(apr_pool_t *p, const char *s, int case_sensitive)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>APR_Util_URI</name>
    <title>URI</title>
    <filename>group___a_p_r___util___u_r_i.html</filename>
    <class kind="struct">apr_uri_t</class>
    <member kind="define">
      <type>#define</type>
      <name>APR_URI_FTP_DEFAULT_PORT</name>
      <anchorfile>group___a_p_r___util___u_r_i.html</anchorfile>
      <anchor>gb9580a0116c1fb0f8162f1f9ab2fd341</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_URI_SSH_DEFAULT_PORT</name>
      <anchorfile>group___a_p_r___util___u_r_i.html</anchorfile>
      <anchor>gedd084cdd64b0be5b588ddc4d6f0416d</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_URI_TELNET_DEFAULT_PORT</name>
      <anchorfile>group___a_p_r___util___u_r_i.html</anchorfile>
      <anchor>g1671c6188f245f7be5624147899da9cb</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_URI_GOPHER_DEFAULT_PORT</name>
      <anchorfile>group___a_p_r___util___u_r_i.html</anchorfile>
      <anchor>g7e8471723227df5c52d92460a2abb889</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_URI_HTTP_DEFAULT_PORT</name>
      <anchorfile>group___a_p_r___util___u_r_i.html</anchorfile>
      <anchor>g463777647d6208d178194adbbbbda5f3</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_URI_POP_DEFAULT_PORT</name>
      <anchorfile>group___a_p_r___util___u_r_i.html</anchorfile>
      <anchor>g137bc7ba4ce4b863feb8e436bc6bdd1f</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_URI_NNTP_DEFAULT_PORT</name>
      <anchorfile>group___a_p_r___util___u_r_i.html</anchorfile>
      <anchor>g0e4055b1902284394fd120e43dd3206e</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_URI_IMAP_DEFAULT_PORT</name>
      <anchorfile>group___a_p_r___util___u_r_i.html</anchorfile>
      <anchor>g53fc37da601746c25359849b27acfef3</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_URI_PROSPERO_DEFAULT_PORT</name>
      <anchorfile>group___a_p_r___util___u_r_i.html</anchorfile>
      <anchor>g7ca1b8eb28912516288a5a5ba42d1b11</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_URI_WAIS_DEFAULT_PORT</name>
      <anchorfile>group___a_p_r___util___u_r_i.html</anchorfile>
      <anchor>ge520a89f6202977fea5b195d2188cf06</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_URI_LDAP_DEFAULT_PORT</name>
      <anchorfile>group___a_p_r___util___u_r_i.html</anchorfile>
      <anchor>gc6ac7b320ccb58f3ad44b5ac490b7f51</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_URI_HTTPS_DEFAULT_PORT</name>
      <anchorfile>group___a_p_r___util___u_r_i.html</anchorfile>
      <anchor>g5ac25d3538574bc716d8127bdb202fbd</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_URI_RTSP_DEFAULT_PORT</name>
      <anchorfile>group___a_p_r___util___u_r_i.html</anchorfile>
      <anchor>gd66b83fc06352a6ba6219c5ae508901c</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_URI_SNEWS_DEFAULT_PORT</name>
      <anchorfile>group___a_p_r___util___u_r_i.html</anchorfile>
      <anchor>g3d3450d81b63780f5de2537ea4a8f96a</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_URI_ACAP_DEFAULT_PORT</name>
      <anchorfile>group___a_p_r___util___u_r_i.html</anchorfile>
      <anchor>ge8a436aedf533d466681ac56e85ccae2</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_URI_NFS_DEFAULT_PORT</name>
      <anchorfile>group___a_p_r___util___u_r_i.html</anchorfile>
      <anchor>gcd280bafe6f95366fa9582eb52cb74d1</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_URI_TIP_DEFAULT_PORT</name>
      <anchorfile>group___a_p_r___util___u_r_i.html</anchorfile>
      <anchor>g381d2bae735b236aa293a3399dcb17d5</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_URI_SIP_DEFAULT_PORT</name>
      <anchorfile>group___a_p_r___util___u_r_i.html</anchorfile>
      <anchor>g91c50d4625b831305cf85e6a3be9dde5</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_URI_UNP_OMITSITEPART</name>
      <anchorfile>group___a_p_r___util___u_r_i.html</anchorfile>
      <anchor>g13feebaa8e2a661a46beae0f8fb69551</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_URI_UNP_OMITUSER</name>
      <anchorfile>group___a_p_r___util___u_r_i.html</anchorfile>
      <anchor>g312de0239cc8f4b137b384f3d34f8ccb</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_URI_UNP_OMITPASSWORD</name>
      <anchorfile>group___a_p_r___util___u_r_i.html</anchorfile>
      <anchor>g7a65861c0175ebe3e163423dc874c3b8</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_URI_UNP_OMITUSERINFO</name>
      <anchorfile>group___a_p_r___util___u_r_i.html</anchorfile>
      <anchor>gb26098299959bf67a7becc479bb33e29</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_URI_UNP_REVEALPASSWORD</name>
      <anchorfile>group___a_p_r___util___u_r_i.html</anchorfile>
      <anchor>g67fbc86193b7fc0c6d4610807004b188</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_URI_UNP_OMITPATHINFO</name>
      <anchorfile>group___a_p_r___util___u_r_i.html</anchorfile>
      <anchor>gadcb0ef8ddcd2aacee457762454ec665</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_URI_UNP_OMITQUERY</name>
      <anchorfile>group___a_p_r___util___u_r_i.html</anchorfile>
      <anchor>g6ead3cdef58aad9828d6eece1a369146</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>apr_uri_t</type>
      <name>apr_uri_t</name>
      <anchorfile>group___a_p_r___util___u_r_i.html</anchorfile>
      <anchor>g612a571fda8dad43c9ccc835165b44b9</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>apr_port_t</type>
      <name>apr_uri_port_of_scheme</name>
      <anchorfile>group___a_p_r___util___u_r_i.html</anchorfile>
      <anchor>g7e1ba9317d01a213ba5aa1660cbfa5f9</anchor>
      <arglist>(const char *scheme_str)</arglist>
    </member>
    <member kind="function">
      <type>char *</type>
      <name>apr_uri_unparse</name>
      <anchorfile>group___a_p_r___util___u_r_i.html</anchorfile>
      <anchor>g063c2a75c575667771232b75e660ef8d</anchor>
      <arglist>(apr_pool_t *p, const apr_uri_t *uptr, unsigned flags)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_uri_parse</name>
      <anchorfile>group___a_p_r___util___u_r_i.html</anchorfile>
      <anchor>gfa0bad4fa506349667a7e95858a06d52</anchor>
      <arglist>(apr_pool_t *p, const char *uri, apr_uri_t *uptr)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_uri_parse_hostinfo</name>
      <anchorfile>group___a_p_r___util___u_r_i.html</anchorfile>
      <anchor>g8572e1555ca46fbebcb4bef0b70043b0</anchor>
      <arglist>(apr_pool_t *p, const char *hostinfo, apr_uri_t *uptr)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>APR_UUID</name>
    <title>UUID Handling</title>
    <filename>group___a_p_r___u_u_i_d.html</filename>
    <class kind="struct">apr_uuid_t</class>
    <member kind="define">
      <type>#define</type>
      <name>APR_UUID_FORMATTED_LENGTH</name>
      <anchorfile>group___a_p_r___u_u_i_d.html</anchorfile>
      <anchor>g2ec982f0e604d3f98f7ee9fffde1bde7</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>apr_uuid_get</name>
      <anchorfile>group___a_p_r___u_u_i_d.html</anchorfile>
      <anchor>g88f83f266e153783fb95382c0d170ca9</anchor>
      <arglist>(apr_uuid_t *uuid)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>apr_uuid_format</name>
      <anchorfile>group___a_p_r___u_u_i_d.html</anchorfile>
      <anchor>g2b148ef181d5500e27a8aefd57dcb677</anchor>
      <arglist>(char *buffer, const apr_uuid_t *uuid)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_uuid_parse</name>
      <anchorfile>group___a_p_r___u_u_i_d.html</anchorfile>
      <anchor>gfb4812de1b199ba2c9101f112acc9aa4</anchor>
      <arglist>(apr_uuid_t *uuid, const char *uuid_str)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>APR_XLATE</name>
    <title>I18N translation library</title>
    <filename>group___a_p_r___x_l_a_t_e.html</filename>
    <member kind="define">
      <type>#define</type>
      <name>APR_DEFAULT_CHARSET</name>
      <anchorfile>group___a_p_r___x_l_a_t_e.html</anchorfile>
      <anchor>gbd7110ed72f4e2427d9f640062a0fc7e</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_LOCALE_CHARSET</name>
      <anchorfile>group___a_p_r___x_l_a_t_e.html</anchorfile>
      <anchor>g994245f7929287b3e5d9102783038c9e</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>apr_xlate_t</type>
      <name>apr_xlate_t</name>
      <anchorfile>group___a_p_r___x_l_a_t_e.html</anchorfile>
      <anchor>g2e43f9a7bf082375a2062a390d2bc4c4</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_xlate_open</name>
      <anchorfile>group___a_p_r___x_l_a_t_e.html</anchorfile>
      <anchor>g9a85ce73945a8644a344b188d7ee7b2c</anchor>
      <arglist>(apr_xlate_t **convset, const char *topage, const char *frompage, apr_pool_t *pool)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_xlate_sb_get</name>
      <anchorfile>group___a_p_r___x_l_a_t_e.html</anchorfile>
      <anchor>g6582b203b7d727ceb1739d8b468edf3e</anchor>
      <arglist>(apr_xlate_t *convset, int *onoff)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_xlate_conv_buffer</name>
      <anchorfile>group___a_p_r___x_l_a_t_e.html</anchorfile>
      <anchor>gd77888846e72130bdacf3f09dafb032f</anchor>
      <arglist>(apr_xlate_t *convset, const char *inbuf, apr_size_t *inbytes_left, char *outbuf, apr_size_t *outbytes_left)</arglist>
    </member>
    <member kind="function">
      <type>apr_int32_t</type>
      <name>apr_xlate_conv_byte</name>
      <anchorfile>group___a_p_r___x_l_a_t_e.html</anchorfile>
      <anchor>g562065db542b253a1b82798d08fa0538</anchor>
      <arglist>(apr_xlate_t *convset, unsigned char inchar)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_xlate_close</name>
      <anchorfile>group___a_p_r___x_l_a_t_e.html</anchorfile>
      <anchor>g5b5b3e59fb5377a94f8210783c76a595</anchor>
      <arglist>(apr_xlate_t *convset)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>APR_Util_XML</name>
    <title>XML</title>
    <filename>group___a_p_r___util___x_m_l.html</filename>
    <namespace>Apache</namespace>
    <class kind="struct">apr_text</class>
    <class kind="struct">apr_text_header</class>
    <class kind="struct">apr_xml_attr</class>
    <class kind="struct">apr_xml_elem</class>
    <class kind="struct">apr_xml_doc</class>
    <member kind="define">
      <type>#define</type>
      <name>APR_XML_NS_DAV_ID</name>
      <anchorfile>group___a_p_r___util___x_m_l.html</anchorfile>
      <anchor>g4b12799e535116c05c40866f9881ff26</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_XML_NS_NONE</name>
      <anchorfile>group___a_p_r___util___x_m_l.html</anchorfile>
      <anchor>g189bc5b5bdd4ff22e92cf2ebe3c8b183</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_XML_NS_ERROR_BASE</name>
      <anchorfile>group___a_p_r___util___x_m_l.html</anchorfile>
      <anchor>ga18113c21d5035c8a75b348fa7ea4f4e</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_XML_NS_IS_ERROR</name>
      <anchorfile>group___a_p_r___util___x_m_l.html</anchorfile>
      <anchor>g4a53f376ee08fb9136d042446882dabf</anchor>
      <arglist>(e)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_XML_ELEM_IS_EMPTY</name>
      <anchorfile>group___a_p_r___util___x_m_l.html</anchorfile>
      <anchor>gb9de2eb1615437e5bb43d38fae0e0b0a</anchor>
      <arglist>(e)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_XML_X2T_FULL</name>
      <anchorfile>group___a_p_r___util___x_m_l.html</anchorfile>
      <anchor>g835b142aab53be5dae76320be483ebc2</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_XML_X2T_INNER</name>
      <anchorfile>group___a_p_r___util___x_m_l.html</anchorfile>
      <anchor>gd1f86b71a111a56019b2107e525e0846</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_XML_X2T_LANG_INNER</name>
      <anchorfile>group___a_p_r___util___x_m_l.html</anchorfile>
      <anchor>g5cbfc5f694421e01008e2676b45aa29d</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_XML_X2T_FULL_NS_LANG</name>
      <anchorfile>group___a_p_r___util___x_m_l.html</anchorfile>
      <anchor>g9b01beaf32a39279d2e066ae75946933</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_XML_GET_URI_ITEM</name>
      <anchorfile>group___a_p_r___util___x_m_l.html</anchorfile>
      <anchor>gbd99120feda9f8c5e6b8209f43f01d67</anchor>
      <arglist>(ary, i)</arglist>
    </member>
    <member kind="typedef">
      <type>apr_text</type>
      <name>apr_text</name>
      <anchorfile>group___a_p_r___util___x_m_l.html</anchorfile>
      <anchor>g2151b852b242e1d8e0ef0b12216884cb</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>apr_text_header</type>
      <name>apr_text_header</name>
      <anchorfile>group___a_p_r___util___x_m_l.html</anchorfile>
      <anchor>g0cbc2fc1d292873ed49490abe9bc44c5</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>apr_xml_attr</type>
      <name>apr_xml_attr</name>
      <anchorfile>group___a_p_r___util___x_m_l.html</anchorfile>
      <anchor>gc162efb6d441aa2a38a63fe7856bdb9e</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>apr_xml_elem</type>
      <name>apr_xml_elem</name>
      <anchorfile>group___a_p_r___util___x_m_l.html</anchorfile>
      <anchor>gb1bb002351dffa1905e12253d71becca</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>apr_xml_doc</type>
      <name>apr_xml_doc</name>
      <anchorfile>group___a_p_r___util___x_m_l.html</anchorfile>
      <anchor>g285ec448df650547ef79f842686c71dd</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>apr_xml_parser</type>
      <name>apr_xml_parser</name>
      <anchorfile>group___a_p_r___util___x_m_l.html</anchorfile>
      <anchor>gd8328a44c7358aebbd45cdf0397253cd</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>apr_text_append</name>
      <anchorfile>group___a_p_r___util___x_m_l.html</anchorfile>
      <anchor>g830e73772b197927fe33023a68037742</anchor>
      <arglist>(apr_pool_t *p, apr_text_header *hdr, const char *text)</arglist>
    </member>
    <member kind="function">
      <type>apr_xml_parser *</type>
      <name>apr_xml_parser_create</name>
      <anchorfile>group___a_p_r___util___x_m_l.html</anchorfile>
      <anchor>gda02ec28d7749957912d0511df6b4f9b</anchor>
      <arglist>(apr_pool_t *pool)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_xml_parse_file</name>
      <anchorfile>group___a_p_r___util___x_m_l.html</anchorfile>
      <anchor>g1c2376c0b60e2b9080420df2beaa3a57</anchor>
      <arglist>(apr_pool_t *p, apr_xml_parser **parser, apr_xml_doc **ppdoc, apr_file_t *xmlfd, apr_size_t buffer_length)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_xml_parser_feed</name>
      <anchorfile>group___a_p_r___util___x_m_l.html</anchorfile>
      <anchor>gaefd789d23ddb657d01be998f808e415</anchor>
      <arglist>(apr_xml_parser *parser, const char *data, apr_size_t len)</arglist>
    </member>
    <member kind="function">
      <type>apr_status_t</type>
      <name>apr_xml_parser_done</name>
      <anchorfile>group___a_p_r___util___x_m_l.html</anchorfile>
      <anchor>g19c8232d9551ac41c392a3fa15844895</anchor>
      <arglist>(apr_xml_parser *parser, apr_xml_doc **pdoc)</arglist>
    </member>
    <member kind="function">
      <type>char *</type>
      <name>apr_xml_parser_geterror</name>
      <anchorfile>group___a_p_r___util___x_m_l.html</anchorfile>
      <anchor>g28a3791c51ab4ba15d0a544d9c4c905c</anchor>
      <arglist>(apr_xml_parser *parser, char *errbuf, apr_size_t errbufsize)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>apr_xml_to_text</name>
      <anchorfile>group___a_p_r___util___x_m_l.html</anchorfile>
      <anchor>gb95b65b64a5770435346905d609dc90d</anchor>
      <arglist>(apr_pool_t *p, const apr_xml_elem *elem, int style, apr_array_header_t *namespaces, int *ns_map, const char **pbuf, apr_size_t *psize)</arglist>
    </member>
    <member kind="function">
      <type>const char *</type>
      <name>apr_xml_empty_elem</name>
      <anchorfile>group___a_p_r___util___x_m_l.html</anchorfile>
      <anchor>g1a952361a96e05e14186c7de01949006</anchor>
      <arglist>(apr_pool_t *p, const apr_xml_elem *elem)</arglist>
    </member>
    <member kind="function">
      <type>const char *</type>
      <name>apr_xml_quote_string</name>
      <anchorfile>group___a_p_r___util___x_m_l.html</anchorfile>
      <anchor>g53c5be9052b94a59f07991c6094a0f42</anchor>
      <arglist>(apr_pool_t *p, const char *s, int quotes)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>apr_xml_quote_elem</name>
      <anchorfile>group___a_p_r___util___x_m_l.html</anchorfile>
      <anchor>g7742bdba87378eaa7a83d97555ae9130</anchor>
      <arglist>(apr_pool_t *p, apr_xml_elem *elem)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>apr_xml_insert_uri</name>
      <anchorfile>group___a_p_r___util___x_m_l.html</anchorfile>
      <anchor>gbcd20a5b9d2e9aec596f61207160b79a</anchor>
      <arglist>(apr_array_header_t *uri_array, const char *uri)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>APR_Util</name>
    <title>APR Utility Functions</title>
    <filename>group___a_p_r___util.html</filename>
    <subgroup>APR_Util_Base64</subgroup>
    <subgroup>APR_Util_Bucket_Brigades</subgroup>
    <subgroup>APR_Util_Date</subgroup>
    <subgroup>APR_Util_DBD</subgroup>
    <subgroup>APR_Util_DBM</subgroup>
    <subgroup>APR_Util_Hook</subgroup>
    <subgroup>APR_Util_LDAP</subgroup>
    <subgroup>APR_Util_MD4</subgroup>
    <subgroup>APR_Util_MC</subgroup>
    <subgroup>APR_Util_Opt</subgroup>
    <subgroup>APR_Util_FIFO</subgroup>
    <subgroup>APR_Util_RL</subgroup>
    <subgroup>APR_Util_RMM</subgroup>
    <subgroup>APR_Util_SSL</subgroup>
    <subgroup>APR_Util_StrMatch</subgroup>
    <subgroup>APR_Util_URI</subgroup>
    <subgroup>APR_Util_XML</subgroup>
    <member kind="define">
      <type>#define</type>
      <name>APU_DECLARE</name>
      <anchorfile>group___a_p_r___util.html</anchorfile>
      <anchor>gece2f256097485c97bf6128097939b77</anchor>
      <arglist>(type)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APU_DECLARE_NONSTD</name>
      <anchorfile>group___a_p_r___util.html</anchorfile>
      <anchor>gd04f1e5573072ea05de9b531d95ef6ab</anchor>
      <arglist>(type)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APU_DECLARE_DATA</name>
      <anchorfile>group___a_p_r___util.html</anchorfile>
      <anchor>g65bfbf306a80269995d815f2d4c98d99</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APU_HAVE_SDBM</name>
      <anchorfile>group___a_p_r___util.html</anchorfile>
      <anchor>g7795e8b8a6dbe17aa418512ae51cc9e1</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APU_HAVE_GDBM</name>
      <anchorfile>group___a_p_r___util.html</anchorfile>
      <anchor>g3eac16005a9e21f22006d59dadb77d66</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APU_HAVE_NDBM</name>
      <anchorfile>group___a_p_r___util.html</anchorfile>
      <anchor>g3fa79ef9715c22453eb7aff83e9267a2</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APU_HAVE_DB</name>
      <anchorfile>group___a_p_r___util.html</anchorfile>
      <anchor>gcf74433d61fbb92af219b1163ea62cae</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APU_HAVE_PGSQL</name>
      <anchorfile>group___a_p_r___util.html</anchorfile>
      <anchor>g33738e2d74cac6a96a75e454cb7f6e8c</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APU_HAVE_MYSQL</name>
      <anchorfile>group___a_p_r___util.html</anchorfile>
      <anchor>g0240dfb3f70de0e139c3e8d4b159a64c</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APU_HAVE_SQLITE3</name>
      <anchorfile>group___a_p_r___util.html</anchorfile>
      <anchor>g67b31da819262f8350eb349d7dacf634</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APU_HAVE_SQLITE2</name>
      <anchorfile>group___a_p_r___util.html</anchorfile>
      <anchor>gb48cbb72a347f6fc52ac9cc82a9518c3</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APU_HAVE_ORACLE</name>
      <anchorfile>group___a_p_r___util.html</anchorfile>
      <anchor>gf7683b132255258cd46318a3ce17907b</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APU_HAVE_APR_ICONV</name>
      <anchorfile>group___a_p_r___util.html</anchorfile>
      <anchor>gee5ce09794f07abdefca5367946d6c68</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APU_HAVE_ICONV</name>
      <anchorfile>group___a_p_r___util.html</anchorfile>
      <anchor>g693af22bf41dab2b96e3a6dce1ea1eb3</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>APR_HAS_XLATE</name>
      <anchorfile>group___a_p_r___util.html</anchorfile>
      <anchor>gac0458354c8b55d292a3977bee6d4485</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>apr_anylock_t</name>
    <filename>structapr__anylock__t.html</filename>
    <member kind="enumeration">
      <name>tm_lock</name>
      <anchor>4d8151ee1fd9cf0fba3cfdc8ebb19b82</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>apr_anylock_none</name>
      <anchor>4d8151ee1fd9cf0fba3cfdc8ebb19b82ae5340702c9e4d2e08a34d57e1811719</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>apr_anylock_procmutex</name>
      <anchor>4d8151ee1fd9cf0fba3cfdc8ebb19b826341436e2dd2c46850920ff5c1ca9f77</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>apr_anylock_threadmutex</name>
      <anchor>4d8151ee1fd9cf0fba3cfdc8ebb19b824b572e72237cf17de682104f32545091</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>apr_anylock_readlock</name>
      <anchor>4d8151ee1fd9cf0fba3cfdc8ebb19b82be92c5b580221c9164d63b760e9ecc35</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>apr_anylock_writelock</name>
      <anchor>4d8151ee1fd9cf0fba3cfdc8ebb19b82179ca70de35c06f267c2931d9487c722</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>enum apr_anylock_t::tm_lock</type>
      <name>type</name>
      <anchorfile>structapr__anylock__t.html</anchorfile>
      <anchor>bb959f658381eab8953ce56321b68ced</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apr_anylock_t::apr_anylock_u_t</type>
      <name>lock</name>
      <anchorfile>structapr__anylock__t.html</anchorfile>
      <anchor>29bd1e1af9d8ff8578e26e376ea7af41</anchor>
      <arglist></arglist>
    </member>
    <class kind="union">apr_anylock_t::apr_anylock_u_t</class>
  </compound>
  <compound kind="union">
    <name>apr_anylock_t::apr_anylock_u_t</name>
    <filename>unionapr__anylock__t_1_1apr__anylock__u__t.html</filename>
    <member kind="variable">
      <type>apr_proc_mutex_t *</type>
      <name>pm</name>
      <anchorfile>unionapr__anylock__t_1_1apr__anylock__u__t.html</anchorfile>
      <anchor>1e1bfdaf49b730301ad12fd9a9d4a954</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apr_thread_mutex_t *</type>
      <name>tm</name>
      <anchorfile>unionapr__anylock__t_1_1apr__anylock__u__t.html</anchorfile>
      <anchor>66e78187f85c6eaf12f8fd6688e8a326</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apr_thread_rwlock_t *</type>
      <name>rw</name>
      <anchorfile>unionapr__anylock__t_1_1apr__anylock__u__t.html</anchorfile>
      <anchor>e51734210eb676c9130db9b7cadde6f3</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>apr_bucket</name>
    <filename>structapr__bucket.html</filename>
    <member kind="function">
      <type></type>
      <name>APR_RING_ENTRY</name>
      <anchorfile>structapr__bucket.html</anchorfile>
      <anchor>bebc478bb76c698f0b3ead7349ff2a97</anchor>
      <arglist>(apr_bucket) link</arglist>
    </member>
    <member kind="variable">
      <type>const apr_bucket_type_t *</type>
      <name>type</name>
      <anchorfile>structapr__bucket.html</anchorfile>
      <anchor>dc6eb719559daed23b067ac08b92a1a0</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apr_size_t</type>
      <name>length</name>
      <anchorfile>structapr__bucket.html</anchorfile>
      <anchor>89b9bd294c41fa6f9cf2ee69e3a48e12</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apr_off_t</type>
      <name>start</name>
      <anchorfile>structapr__bucket.html</anchorfile>
      <anchor>339ce98f5584eb9066645b4bd661d622</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>void *</type>
      <name>data</name>
      <anchorfile>structapr__bucket.html</anchorfile>
      <anchor>55cdc5fd2890a22589f7958e8bdc71e6</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>void(*</type>
      <name>free</name>
      <anchorfile>structapr__bucket.html</anchorfile>
      <anchor>58067b5fed835cca64dacc73441745eb</anchor>
      <arglist>)(void *e)</arglist>
    </member>
    <member kind="variable">
      <type>apr_bucket_alloc_t *</type>
      <name>list</name>
      <anchorfile>structapr__bucket.html</anchorfile>
      <anchor>ee2ec9359e7efffb3f6dfea4191e761e</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>apr_bucket_brigade</name>
    <filename>structapr__bucket__brigade.html</filename>
    <member kind="function">
      <type></type>
      <name>APR_RING_HEAD</name>
      <anchorfile>structapr__bucket__brigade.html</anchorfile>
      <anchor>a201dccab2ddbd183763bc55201d07d8</anchor>
      <arglist>(apr_bucket_list, apr_bucket) list</arglist>
    </member>
    <member kind="variable">
      <type>apr_pool_t *</type>
      <name>p</name>
      <anchorfile>structapr__bucket__brigade.html</anchorfile>
      <anchor>a5c9768111bdde1c38b9e68ad26f5e02</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apr_bucket_alloc_t *</type>
      <name>bucket_alloc</name>
      <anchorfile>structapr__bucket__brigade.html</anchorfile>
      <anchor>0a2a9cd4b521466e7f8299304f128723</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>apr_bucket_file</name>
    <filename>structapr__bucket__file.html</filename>
    <member kind="variable">
      <type>apr_bucket_refcount</type>
      <name>refcount</name>
      <anchorfile>structapr__bucket__file.html</anchorfile>
      <anchor>59078c99052f8734cc410132c31ef9ed</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apr_file_t *</type>
      <name>fd</name>
      <anchorfile>structapr__bucket__file.html</anchorfile>
      <anchor>37cdcac5c017a9bb100e6f76780677ee</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apr_pool_t *</type>
      <name>readpool</name>
      <anchorfile>structapr__bucket__file.html</anchorfile>
      <anchor>7e741e89ae3a836a6c4182961e91cb0f</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int</type>
      <name>can_mmap</name>
      <anchorfile>structapr__bucket__file.html</anchorfile>
      <anchor>855ac7c371f80009a4d107b294e6ff6b</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>apr_bucket_heap</name>
    <filename>structapr__bucket__heap.html</filename>
    <member kind="variable">
      <type>apr_bucket_refcount</type>
      <name>refcount</name>
      <anchorfile>structapr__bucket__heap.html</anchorfile>
      <anchor>070cffdb52364fc6ee555f0111bbeada</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>char *</type>
      <name>base</name>
      <anchorfile>structapr__bucket__heap.html</anchorfile>
      <anchor>182f938bd494ac0f1fd6d9b8005d5a34</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apr_size_t</type>
      <name>alloc_len</name>
      <anchorfile>structapr__bucket__heap.html</anchorfile>
      <anchor>ca97d1d1b515388ef602324131ab3e3a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>void(*</type>
      <name>free_func</name>
      <anchorfile>structapr__bucket__heap.html</anchorfile>
      <anchor>30f43c180304a6308234c7aa1744e840</anchor>
      <arglist>)(void *data)</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>apr_bucket_mmap</name>
    <filename>structapr__bucket__mmap.html</filename>
    <member kind="variable">
      <type>apr_bucket_refcount</type>
      <name>refcount</name>
      <anchorfile>structapr__bucket__mmap.html</anchorfile>
      <anchor>131ecfd7de5cb6e30f78019e03ce79d5</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apr_mmap_t *</type>
      <name>mmap</name>
      <anchorfile>structapr__bucket__mmap.html</anchorfile>
      <anchor>7cab29e0257cc146c8e9f8105cdacc88</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>apr_bucket_pool</name>
    <filename>structapr__bucket__pool.html</filename>
    <member kind="variable">
      <type>apr_bucket_heap</type>
      <name>heap</name>
      <anchorfile>structapr__bucket__pool.html</anchorfile>
      <anchor>823bbd0794d19d861d4d94b41638aa70</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char *</type>
      <name>base</name>
      <anchorfile>structapr__bucket__pool.html</anchorfile>
      <anchor>c9e46b2b3c453e8db8f90e3c41ebf6c7</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apr_pool_t *</type>
      <name>pool</name>
      <anchorfile>structapr__bucket__pool.html</anchorfile>
      <anchor>f88dc3bd0c24b6b08494de593b11a61a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apr_bucket_alloc_t *</type>
      <name>list</name>
      <anchorfile>structapr__bucket__pool.html</anchorfile>
      <anchor>7049f191bac1c65b65324b4a6190c5d9</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>apr_bucket_refcount</name>
    <filename>structapr__bucket__refcount.html</filename>
    <member kind="variable">
      <type>int</type>
      <name>refcount</name>
      <anchorfile>structapr__bucket__refcount.html</anchorfile>
      <anchor>c309a824add9da08ea1551b4faffc84b</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="union">
    <name>apr_bucket_structs</name>
    <filename>unionapr__bucket__structs.html</filename>
    <member kind="variable">
      <type>apr_bucket</type>
      <name>b</name>
      <anchorfile>unionapr__bucket__structs.html</anchorfile>
      <anchor>b103f79c9e6ef5602170bc8ade19bc85</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apr_bucket_heap</type>
      <name>heap</name>
      <anchorfile>unionapr__bucket__structs.html</anchorfile>
      <anchor>4d689325457f3350d063734b7d8050cc</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apr_bucket_pool</type>
      <name>pool</name>
      <anchorfile>unionapr__bucket__structs.html</anchorfile>
      <anchor>975cf2c98a7aa4024e0c46ee02e3bd6d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apr_bucket_mmap</type>
      <name>mmap</name>
      <anchorfile>unionapr__bucket__structs.html</anchorfile>
      <anchor>800720f64dc9819bf92ec0442a98e40a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apr_bucket_file</type>
      <name>file</name>
      <anchorfile>unionapr__bucket__structs.html</anchorfile>
      <anchor>4390135d15cc2abbcb622c856b01b0fd</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>apr_bucket_type_t</name>
    <filename>structapr__bucket__type__t.html</filename>
    <member kind="enumvalue">
      <name>APR_BUCKET_DATA</name>
      <anchor>0b9e887de9622ebdf6bfad6ae6c725e2fa492c8a0f00e1540092143986daa0b0</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>APR_BUCKET_METADATA</name>
      <anchor>0b9e887de9622ebdf6bfad6ae6c725e2659393f1582e65e92846a976140f4a95</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char *</type>
      <name>name</name>
      <anchorfile>structapr__bucket__type__t.html</anchorfile>
      <anchor>ef14e786a1ba77c43eed89eef6c2c11b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int</type>
      <name>num_func</name>
      <anchorfile>structapr__bucket__type__t.html</anchorfile>
      <anchor>2dcc213b22bebd42b74ef5ebf0f70e64</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>enum apr_bucket_type_t::@0</type>
      <name>is_metadata</name>
      <anchorfile>structapr__bucket__type__t.html</anchorfile>
      <anchor>2506e3a7e5629cea0350e10ba4beeec7</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>void(*</type>
      <name>destroy</name>
      <anchorfile>structapr__bucket__type__t.html</anchorfile>
      <anchor>0b5bde430fc8bd699b93f713f078a8b3</anchor>
      <arglist>)(void *data)</arglist>
    </member>
    <member kind="variable">
      <type>apr_status_t(*</type>
      <name>read</name>
      <anchorfile>structapr__bucket__type__t.html</anchorfile>
      <anchor>3f6b91a11910e47494c1710acf2dbc78</anchor>
      <arglist>)(apr_bucket *b, const char **str, apr_size_t *len, apr_read_type_e block)</arglist>
    </member>
    <member kind="variable">
      <type>apr_status_t(*</type>
      <name>setaside</name>
      <anchorfile>structapr__bucket__type__t.html</anchorfile>
      <anchor>22125c62e9377cae847620e0b7c7d40a</anchor>
      <arglist>)(apr_bucket *e, apr_pool_t *pool)</arglist>
    </member>
    <member kind="variable">
      <type>apr_status_t(*</type>
      <name>split</name>
      <anchorfile>structapr__bucket__type__t.html</anchorfile>
      <anchor>fc66f5b5fa17bc1f5065b915964db423</anchor>
      <arglist>)(apr_bucket *e, apr_size_t point)</arglist>
    </member>
    <member kind="variable">
      <type>apr_status_t(*</type>
      <name>copy</name>
      <anchorfile>structapr__bucket__type__t.html</anchorfile>
      <anchor>baa0299c333fbacd2a7776c10e4a6ff4</anchor>
      <arglist>)(apr_bucket *e, apr_bucket **c)</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>apr_datum_t</name>
    <filename>structapr__datum__t.html</filename>
    <member kind="variable">
      <type>char *</type>
      <name>dptr</name>
      <anchorfile>structapr__datum__t.html</anchorfile>
      <anchor>9a4974f5a51856f61bb3d51e2a7755d6</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apr_size_t</type>
      <name>dsize</name>
      <anchorfile>structapr__datum__t.html</anchorfile>
      <anchor>5e418ab877cb96b26a5384ebd36bff6e</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>apr_dbm_t</name>
    <filename>structapr__dbm__t.html</filename>
    <member kind="variable">
      <type>apr_pool_t *</type>
      <name>pool</name>
      <anchorfile>structapr__dbm__t.html</anchorfile>
      <anchor>c776319b598398d6a7d494826d64bcc2</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>void *</type>
      <name>file</name>
      <anchorfile>structapr__dbm__t.html</anchorfile>
      <anchor>5640144f3c99fc9e24ed6b6b156ef7d2</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int</type>
      <name>errcode</name>
      <anchorfile>structapr__dbm__t.html</anchorfile>
      <anchor>bc6389361f0fa3b83ce1ac677a7226df</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char *</type>
      <name>errmsg</name>
      <anchorfile>structapr__dbm__t.html</anchorfile>
      <anchor>539ba3b9011e1585abde116f5dcc89f2</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const apr_dbm_type_t *</type>
      <name>type</name>
      <anchorfile>structapr__dbm__t.html</anchorfile>
      <anchor>fa11e6dcdc6f9239142813ea4f1c92f9</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>apr_dbm_type_t</name>
    <filename>structapr__dbm__type__t.html</filename>
    <member kind="variable">
      <type>const char *</type>
      <name>name</name>
      <anchorfile>structapr__dbm__type__t.html</anchorfile>
      <anchor>11d14e7266d939f4d5e2699d021e1317</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apr_status_t(*</type>
      <name>open</name>
      <anchorfile>structapr__dbm__type__t.html</anchorfile>
      <anchor>237cee1f81751031fe5c89f7718d8380</anchor>
      <arglist>)(apr_dbm_t **pdb, const char *pathname, apr_int32_t mode, apr_fileperms_t perm, apr_pool_t *pool)</arglist>
    </member>
    <member kind="variable">
      <type>void(*</type>
      <name>close</name>
      <anchorfile>structapr__dbm__type__t.html</anchorfile>
      <anchor>91f207e265059561291fd7187bea4fd6</anchor>
      <arglist>)(apr_dbm_t *dbm)</arglist>
    </member>
    <member kind="variable">
      <type>apr_status_t(*</type>
      <name>fetch</name>
      <anchorfile>structapr__dbm__type__t.html</anchorfile>
      <anchor>a9f68252fc35d38b2b9bb8779f6ee10d</anchor>
      <arglist>)(apr_dbm_t *dbm, apr_datum_t key, apr_datum_t *pvalue)</arglist>
    </member>
    <member kind="variable">
      <type>apr_status_t(*</type>
      <name>store</name>
      <anchorfile>structapr__dbm__type__t.html</anchorfile>
      <anchor>f9f440535e13dcb130266697954392c2</anchor>
      <arglist>)(apr_dbm_t *dbm, apr_datum_t key, apr_datum_t value)</arglist>
    </member>
    <member kind="variable">
      <type>apr_status_t(*</type>
      <name>del</name>
      <anchorfile>structapr__dbm__type__t.html</anchorfile>
      <anchor>e0295118eede11b264c80ff647593a37</anchor>
      <arglist>)(apr_dbm_t *dbm, apr_datum_t key)</arglist>
    </member>
    <member kind="variable">
      <type>int(*</type>
      <name>exists</name>
      <anchorfile>structapr__dbm__type__t.html</anchorfile>
      <anchor>15e031162b7ae6ed002488bf3f1f1d6f</anchor>
      <arglist>)(apr_dbm_t *dbm, apr_datum_t key)</arglist>
    </member>
    <member kind="variable">
      <type>apr_status_t(*</type>
      <name>firstkey</name>
      <anchorfile>structapr__dbm__type__t.html</anchorfile>
      <anchor>0911853c53c2d681f703041f4e97e7e1</anchor>
      <arglist>)(apr_dbm_t *dbm, apr_datum_t *pkey)</arglist>
    </member>
    <member kind="variable">
      <type>apr_status_t(*</type>
      <name>nextkey</name>
      <anchorfile>structapr__dbm__type__t.html</anchorfile>
      <anchor>09c99cc033aed5bb631e32a3f0eff4f2</anchor>
      <arglist>)(apr_dbm_t *dbm, apr_datum_t *pkey)</arglist>
    </member>
    <member kind="variable">
      <type>void(*</type>
      <name>freedatum</name>
      <anchorfile>structapr__dbm__type__t.html</anchorfile>
      <anchor>4c314b45372bff56b908b651845d714a</anchor>
      <arglist>)(apr_dbm_t *dbm, apr_datum_t data)</arglist>
    </member>
    <member kind="variable">
      <type>void(*</type>
      <name>getusednames</name>
      <anchorfile>structapr__dbm__type__t.html</anchorfile>
      <anchor>63a40c9e503a7d280207d4adcb8504b6</anchor>
      <arglist>)(apr_pool_t *pool, const char *pathname, const char **used1, const char **used2)</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>apr_md4_ctx_t</name>
    <filename>structapr__md4__ctx__t.html</filename>
    <member kind="variable">
      <type>apr_uint32_t</type>
      <name>state</name>
      <anchorfile>structapr__md4__ctx__t.html</anchorfile>
      <anchor>54f4563ec350fa9fd088f958a5dc71e0</anchor>
      <arglist>[4]</arglist>
    </member>
    <member kind="variable">
      <type>apr_uint32_t</type>
      <name>count</name>
      <anchorfile>structapr__md4__ctx__t.html</anchorfile>
      <anchor>ff81f0b511c228887667b451b66cbc0a</anchor>
      <arglist>[2]</arglist>
    </member>
    <member kind="variable">
      <type>unsigned char</type>
      <name>buffer</name>
      <anchorfile>structapr__md4__ctx__t.html</anchorfile>
      <anchor>e58e87318b86d7e74cade0d3c59c6e50</anchor>
      <arglist>[64]</arglist>
    </member>
    <member kind="variable">
      <type>apr_xlate_t *</type>
      <name>xlate</name>
      <anchorfile>structapr__md4__ctx__t.html</anchorfile>
      <anchor>c6aa4d0c11d3b35f70c2a50d729532ad</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>apr_md5_ctx_t</name>
    <filename>structapr__md5__ctx__t.html</filename>
    <member kind="variable">
      <type>apr_uint32_t</type>
      <name>state</name>
      <anchorfile>structapr__md5__ctx__t.html</anchorfile>
      <anchor>b867226a3ed8230df2be621247d50a0b</anchor>
      <arglist>[4]</arglist>
    </member>
    <member kind="variable">
      <type>apr_uint32_t</type>
      <name>count</name>
      <anchorfile>structapr__md5__ctx__t.html</anchorfile>
      <anchor>ba5095175ca14745746f3bfcc0febfbf</anchor>
      <arglist>[2]</arglist>
    </member>
    <member kind="variable">
      <type>unsigned char</type>
      <name>buffer</name>
      <anchorfile>structapr__md5__ctx__t.html</anchorfile>
      <anchor>f014c25cec100a8b936f32f98c3fae7a</anchor>
      <arglist>[64]</arglist>
    </member>
    <member kind="variable">
      <type>apr_xlate_t *</type>
      <name>xlate</name>
      <anchorfile>structapr__md5__ctx__t.html</anchorfile>
      <anchor>e059bdb70ede2561884e2cd045f2a757</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>apr_memcache_t</name>
    <filename>structapr__memcache__t.html</filename>
    <member kind="variable">
      <type>apr_uint32_t</type>
      <name>flags</name>
      <anchorfile>structapr__memcache__t.html</anchorfile>
      <anchor>a92a7349ba67dcce219007781247c0ef</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apr_uint16_t</type>
      <name>nalloc</name>
      <anchorfile>structapr__memcache__t.html</anchorfile>
      <anchor>dca3605c99bf8a5f9d72146d50514cf5</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apr_uint16_t</type>
      <name>ntotal</name>
      <anchorfile>structapr__memcache__t.html</anchorfile>
      <anchor>1f2dd0a8f9fc58d4fc9555fd7f0d3d3d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apr_memcache_server_t **</type>
      <name>live_servers</name>
      <anchorfile>structapr__memcache__t.html</anchorfile>
      <anchor>6737ab9afbe583ce6dac1444f05ccfc0</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apr_pool_t *</type>
      <name>p</name>
      <anchorfile>structapr__memcache__t.html</anchorfile>
      <anchor>a80bdbcb2671d4e27c2bd4a49ab9932c</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>apr_memcache_value_t</name>
    <filename>structapr__memcache__value__t.html</filename>
    <member kind="variable">
      <type>apr_status_t</type>
      <name>status</name>
      <anchorfile>structapr__memcache__value__t.html</anchorfile>
      <anchor>5c1066bde5435b2c69375719eab0d013</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char *</type>
      <name>key</name>
      <anchorfile>structapr__memcache__value__t.html</anchorfile>
      <anchor>c1cf5b982ce0d5e6c727f71b33e1ee17</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apr_size_t</type>
      <name>len</name>
      <anchorfile>structapr__memcache__value__t.html</anchorfile>
      <anchor>8da67f285b82f952b59c9c5c093c14f7</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>char *</type>
      <name>data</name>
      <anchorfile>structapr__memcache__value__t.html</anchorfile>
      <anchor>53475973d4dec275be20ce1fc0cfad1b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apr_uint16_t</type>
      <name>flags</name>
      <anchorfile>structapr__memcache__value__t.html</anchorfile>
      <anchor>dd8d49654724ebd26e9a4cefc56dcee7</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>apr_sdbm_datum_t</name>
    <filename>structapr__sdbm__datum__t.html</filename>
    <member kind="variable">
      <type>char *</type>
      <name>dptr</name>
      <anchorfile>structapr__sdbm__datum__t.html</anchorfile>
      <anchor>c1d5081a2ea070b836207e4b7de3f5c2</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int</type>
      <name>dsize</name>
      <anchorfile>structapr__sdbm__datum__t.html</anchorfile>
      <anchor>60a19d8656aab6051b7b7cd1cfcf34eb</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>apr_sha1_ctx_t</name>
    <filename>structapr__sha1__ctx__t.html</filename>
    <member kind="variable">
      <type>apr_uint32_t</type>
      <name>digest</name>
      <anchorfile>structapr__sha1__ctx__t.html</anchorfile>
      <anchor>aff723c78a82857aa4540f75b5078bee</anchor>
      <arglist>[5]</arglist>
    </member>
    <member kind="variable">
      <type>apr_uint32_t</type>
      <name>count_lo</name>
      <anchorfile>structapr__sha1__ctx__t.html</anchorfile>
      <anchor>801990504e67b0780732524adc4977f7</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apr_uint32_t</type>
      <name>count_hi</name>
      <anchorfile>structapr__sha1__ctx__t.html</anchorfile>
      <anchor>926f7b2bedb422e889272211ca90e450</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apr_uint32_t</type>
      <name>data</name>
      <anchorfile>structapr__sha1__ctx__t.html</anchorfile>
      <anchor>393c6ba66066088861753035ed1e484d</anchor>
      <arglist>[16]</arglist>
    </member>
    <member kind="variable">
      <type>int</type>
      <name>local</name>
      <anchorfile>structapr__sha1__ctx__t.html</anchorfile>
      <anchor>814094552c53f9131d10f1cef05071f6</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>apr_ssl_factory</name>
    <filename>structapr__ssl__factory.html</filename>
    <member kind="variable">
      <type>apr_pool_t *</type>
      <name>pool</name>
      <anchorfile>structapr__ssl__factory.html</anchorfile>
      <anchor>857df969577b8f33d3a8f0ee144dd066</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apr_ssl_factory_type_e</type>
      <name>purpose</name>
      <anchorfile>structapr__ssl__factory.html</anchorfile>
      <anchor>d7bf13b7e73382a92a053055beee37dd</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apu_ssl_data_t *</type>
      <name>sslData</name>
      <anchorfile>structapr__ssl__factory.html</anchorfile>
      <anchor>90c166bdb1a2b3a59d3a5ad16e6bd255</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>apr_ssl_socket</name>
    <filename>structapr__ssl__socket.html</filename>
    <member kind="variable">
      <type>apr_pool_t *</type>
      <name>pool</name>
      <anchorfile>structapr__ssl__socket.html</anchorfile>
      <anchor>3b6e3f350f989b13a30d8fce37be2a46</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apr_socket_t *</type>
      <name>plain</name>
      <anchorfile>structapr__ssl__socket.html</anchorfile>
      <anchor>02bdc5a5994d58f6612612c6f64bde3d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apr_ssl_factory_t *</type>
      <name>factory</name>
      <anchorfile>structapr__ssl__socket.html</anchorfile>
      <anchor>13fcbbd7b7d43bb6c2a2419f831a5938</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apr_pollset_t *</type>
      <name>pollset</name>
      <anchorfile>structapr__ssl__socket.html</anchorfile>
      <anchor>b7c999d4f893d054d660cefd4565567d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apr_pollfd_t *</type>
      <name>poll</name>
      <anchorfile>structapr__ssl__socket.html</anchorfile>
      <anchor>5dc93f631fc99b1839c9a1e9d7df298c</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int</type>
      <name>connected</name>
      <anchorfile>structapr__ssl__socket.html</anchorfile>
      <anchor>62c74a6f83f981907c5ece61cd74e715</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apu_ssl_socket_data_t *</type>
      <name>sslData</name>
      <anchorfile>structapr__ssl__socket.html</anchorfile>
      <anchor>12d4fa2bb2b5268c3cea67352204e3b3</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>apr_strmatch_pattern</name>
    <filename>structapr__strmatch__pattern.html</filename>
    <member kind="variable">
      <type>const char *(*</type>
      <name>compare</name>
      <anchorfile>structapr__strmatch__pattern.html</anchorfile>
      <anchor>524e25aefd31df69ac44bc60b20feedb</anchor>
      <arglist>)(const apr_strmatch_pattern *this_pattern, const char *s, apr_size_t slen)</arglist>
    </member>
    <member kind="variable">
      <type>const char *</type>
      <name>pattern</name>
      <anchorfile>structapr__strmatch__pattern.html</anchorfile>
      <anchor>ad1fbc5c82b2903e14db83695cff8801</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apr_size_t</type>
      <name>length</name>
      <anchorfile>structapr__strmatch__pattern.html</anchorfile>
      <anchor>43beede5de17078529020a4e9a8792fe</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>void *</type>
      <name>context</name>
      <anchorfile>structapr__strmatch__pattern.html</anchorfile>
      <anchor>356c5739f71c8258b1b57806d7797c8c</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>apr_text</name>
    <filename>structapr__text.html</filename>
    <member kind="variable">
      <type>const char *</type>
      <name>text</name>
      <anchorfile>structapr__text.html</anchorfile>
      <anchor>366805efa97abdcd3466b7041c787296</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apr_text *</type>
      <name>next</name>
      <anchorfile>structapr__text.html</anchorfile>
      <anchor>c4b5010962ad304dbc621ea639dba9a1</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>apr_text_header</name>
    <filename>structapr__text__header.html</filename>
    <member kind="variable">
      <type>apr_text *</type>
      <name>first</name>
      <anchorfile>structapr__text__header.html</anchorfile>
      <anchor>eded422c282ed9d4feb3058723800f91</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apr_text *</type>
      <name>last</name>
      <anchorfile>structapr__text__header.html</anchorfile>
      <anchor>e0f746327d3e04f88d793ab18a25f258</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>apr_uri_t</name>
    <filename>structapr__uri__t.html</filename>
    <member kind="variable">
      <type>char *</type>
      <name>scheme</name>
      <anchorfile>structapr__uri__t.html</anchorfile>
      <anchor>68bc62e046b8ca45fc78d4f2316c4d78</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>char *</type>
      <name>hostinfo</name>
      <anchorfile>structapr__uri__t.html</anchorfile>
      <anchor>e5175153d907a127810995f04cff0dcb</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>char *</type>
      <name>user</name>
      <anchorfile>structapr__uri__t.html</anchorfile>
      <anchor>a12d1ac3431d8cd6899e13f27c5ceb69</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>char *</type>
      <name>password</name>
      <anchorfile>structapr__uri__t.html</anchorfile>
      <anchor>0c4fc4c9df4b75073692e7d2b37e210f</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>char *</type>
      <name>hostname</name>
      <anchorfile>structapr__uri__t.html</anchorfile>
      <anchor>b21a3a241f20a7f7a75ca0629748272d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>char *</type>
      <name>port_str</name>
      <anchorfile>structapr__uri__t.html</anchorfile>
      <anchor>cd61a358fcbcfb3fb1002f2944e8cf00</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>char *</type>
      <name>path</name>
      <anchorfile>structapr__uri__t.html</anchorfile>
      <anchor>ab63b9da67f8ce50229585138a5114bf</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>char *</type>
      <name>query</name>
      <anchorfile>structapr__uri__t.html</anchorfile>
      <anchor>f2b2f207f4191cb8dea29f657589cf70</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>char *</type>
      <name>fragment</name>
      <anchorfile>structapr__uri__t.html</anchorfile>
      <anchor>8e8ab8a61f01feec2f3624f63152a5ed</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>hostent *</type>
      <name>hostent</name>
      <anchorfile>structapr__uri__t.html</anchorfile>
      <anchor>85ded046fa45022d6cc803383a8ebbef</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apr_port_t</type>
      <name>port</name>
      <anchorfile>structapr__uri__t.html</anchorfile>
      <anchor>826eebe83cc45f1803fecec109df678d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>unsigned</type>
      <name>is_initialized</name>
      <anchorfile>structapr__uri__t.html</anchorfile>
      <anchor>c011400ce9cf220f87abcee2a7329ca2</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>unsigned</type>
      <name>dns_looked_up</name>
      <anchorfile>structapr__uri__t.html</anchorfile>
      <anchor>b8589a85afaa90b19dfd83f4647e7fde</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>unsigned</type>
      <name>dns_resolved</name>
      <anchorfile>structapr__uri__t.html</anchorfile>
      <anchor>1175867685dcd6cc7e70d203f9cd4efd</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>apr_uuid_t</name>
    <filename>structapr__uuid__t.html</filename>
    <member kind="variable">
      <type>unsigned char</type>
      <name>data</name>
      <anchorfile>structapr__uuid__t.html</anchorfile>
      <anchor>2cb4092898cc4d98a255c7ce1ae35690</anchor>
      <arglist>[16]</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>apr_xml_attr</name>
    <filename>structapr__xml__attr.html</filename>
    <member kind="variable">
      <type>const char *</type>
      <name>name</name>
      <anchorfile>structapr__xml__attr.html</anchorfile>
      <anchor>734fa5952a25b2f8afa05ab6747d523e</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int</type>
      <name>ns</name>
      <anchorfile>structapr__xml__attr.html</anchorfile>
      <anchor>fde916479d3b67228d5d460f7417bfe6</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char *</type>
      <name>value</name>
      <anchorfile>structapr__xml__attr.html</anchorfile>
      <anchor>7a543680eddb9a40d22193e90e658f66</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apr_xml_attr *</type>
      <name>next</name>
      <anchorfile>structapr__xml__attr.html</anchorfile>
      <anchor>b50cd3a6e161c4c0f1fdbc8d8bba994e</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>apr_xml_doc</name>
    <filename>structapr__xml__doc.html</filename>
    <member kind="variable">
      <type>apr_xml_elem *</type>
      <name>root</name>
      <anchorfile>structapr__xml__doc.html</anchorfile>
      <anchor>a8f95013263a41722689461fe923e29a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apr_array_header_t *</type>
      <name>namespaces</name>
      <anchorfile>structapr__xml__doc.html</anchorfile>
      <anchor>5338037c01fd3db202b45668074f60e0</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>apr_xml_elem</name>
    <filename>structapr__xml__elem.html</filename>
    <member kind="variable">
      <type>const char *</type>
      <name>name</name>
      <anchorfile>structapr__xml__elem.html</anchorfile>
      <anchor>b54a31ae72691bdfafe3090e1e6e954e</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int</type>
      <name>ns</name>
      <anchorfile>structapr__xml__elem.html</anchorfile>
      <anchor>052be3dffa2b3073fccbf041eb298e5b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char *</type>
      <name>lang</name>
      <anchorfile>structapr__xml__elem.html</anchorfile>
      <anchor>734efedd3a4bc726c8dd53ef707fdb66</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apr_text_header</type>
      <name>first_cdata</name>
      <anchorfile>structapr__xml__elem.html</anchorfile>
      <anchor>440014f1ca30d2dfe548df1832267910</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apr_text_header</type>
      <name>following_cdata</name>
      <anchorfile>structapr__xml__elem.html</anchorfile>
      <anchor>59dbc299a647dddb6971d4abd39798ba</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apr_xml_elem *</type>
      <name>parent</name>
      <anchorfile>structapr__xml__elem.html</anchorfile>
      <anchor>22068d5b8d15f5b07967fce93ce573f3</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apr_xml_elem *</type>
      <name>next</name>
      <anchorfile>structapr__xml__elem.html</anchorfile>
      <anchor>40a22de5d6784236c8e6d9bfc1c9da87</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apr_xml_elem *</type>
      <name>first_child</name>
      <anchorfile>structapr__xml__elem.html</anchorfile>
      <anchor>0630b5a811c68f32b804e1231e13f210</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apr_xml_attr *</type>
      <name>attr</name>
      <anchorfile>structapr__xml__elem.html</anchorfile>
      <anchor>74c486ae7150af8b212df93666c41356</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apr_xml_elem *</type>
      <name>last_child</name>
      <anchorfile>structapr__xml__elem.html</anchorfile>
      <anchor>945bd57787cede282b84d8d4b37970d8</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>apr_xml_ns_scope *</type>
      <name>ns_scope</name>
      <anchorfile>structapr__xml__elem.html</anchorfile>
      <anchor>16c3aba915f2f94d4a3f1f38ee32970f</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>void *</type>
      <name>priv</name>
      <anchorfile>structapr__xml__elem.html</anchorfile>
      <anchor>ecb5a2e562f5747a8519f8a370bc8f7e</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="namespace">
    <name>Apache</name>
    <filename>namespace_apache.html</filename>
  </compound>
</tagfile>
