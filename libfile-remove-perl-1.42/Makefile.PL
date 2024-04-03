use inc::Module::Install 0.76;

all_from      'lib/File/Remove.pm';
requires      'File::Copy' => 0;
requires      'File::Path' => 0;
requires      'File::Glob' => 0;
requires      'File::Spec' => '0.80';
test_requires 'Test::More' => '0.42';

WriteAll;
