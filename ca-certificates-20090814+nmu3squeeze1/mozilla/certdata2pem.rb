#!/usr/bin/ruby

while line = $stdin.gets
  next if line =~ /^#/
  if line =~ /^\s*$/
     fname = nil
     next
  end
  line.chomp!
  if line =~ /CKA_LABEL/
    label,type,val = line.split(' ',3)
    val.sub!(/^"/, "")
    val.sub!(/"$/, "")
    fname = val.gsub(/\//,"_").gsub(/\s+/, "_").gsub(/[()]/, "=").gsub(/,/, "_") + ".crt"
    next
  end
  if line =~ /CKA_VALUE MULTILINE_OCTAL/
    if fname.nil?
      puts "E: unexpected CKA_VALUE MULTILINE_OCTAL"
      next
    end
    data=''
    while line = $stdin.gets
      break if line =~ /^END/
      line.chomp!
      line.gsub(/\\([0-3][0-7][0-7])/) { data += $1.oct.chr }
    end
    open(fname, "w") do |fp|
      fp.puts "-----BEGIN CERTIFICATE-----"
      fp.puts [data].pack("m*")
      fp.puts "-----END CERTIFICATE-----"
    end
    puts "Created #{fname}"
  end
end
# system("c_rehash", ".")
