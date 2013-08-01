PROJECT_CEEDLING_ROOT = "../../tools/ceedling"
load "#{PROJECT_CEEDLING_ROOT}/lib/rakefile.rb"

task :shared do
	sh "(cd ../shared/pic12/; rake ;)"
end

task :clean_shared do
	sh "(cd ../shared/pic12/; rake clean;)"
end

task :default => %w[ clean test:all release ]
task :full => %w[ clean clean_shared default ]