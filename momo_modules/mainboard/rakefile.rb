PROJECT_CEEDLING_ROOT = "tools/ceedling"
load "#{PROJECT_CEEDLING_ROOT}/lib/rakefile.rb"

task :default => %w[ test:all release ]
task :full => %w[ clean test:all release ]
