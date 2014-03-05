PROJECT_CEEDLING_ROOT = "../../tools/ceedling"
load "#{PROJECT_CEEDLING_ROOT}/lib/rakefile.rb"

task :incremental => %w[ release ]
task :full => %w[ clean incremental ]

task :default => :full
