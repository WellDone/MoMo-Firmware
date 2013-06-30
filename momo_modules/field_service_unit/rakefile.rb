PROJECT_CEEDLING_ROOT = "../../tools/ceedling"
load "#{PROJECT_CEEDLING_ROOT}/lib/rakefile.rb"

task :shared do
	sh "(cd ../shared/pic24/; rake;)"
end

task :clean_shared do
	sh "(cd ../shared/pic24/; rake clean;)"
end

task :incremental => %w[ shared test:all release ]
task :full => %w[ clean clean_shared incremental ]

task :default => :full
