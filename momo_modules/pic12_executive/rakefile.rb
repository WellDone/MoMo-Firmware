PROJECT_CEEDLING_ROOT = "../../tools/ceedling"
load "#{PROJECT_CEEDLING_ROOT}/lib/rakefile.rb"

task :shared do
	sh "(cd ../shared/pic12/; rake ;)"
end

directory "build"

task :clean_shared do
	sh "(cd ../shared/pic12/; rake clean;)"
end

task :pic12 do
	ENV['PIC_DEVICE_DEFINE']="__PIC12LF1822__"
	sh "(echo $PIC_DEVICE_DEFINE; rake noshared)"
	sh "mkdir ./build_pic12; mv ./build/release ./build_pic12; mv ./build/test ./build_pic12";
end
task :pic16 do
	ENV['PIC_DEVICE_DEFINE']="__PIC16LF1823__"
	sh "(echo $PIC_DEVICE_DEFINE; rake noshared)"
	sh "mkdir ./build_pic16; mv ./build/release ./build_pic16; mv ./build/test ./build_pic16";
end

task :noshared => %w[ build test:all release ]
task :incremental => %w[ shared noshared ]
task :full => %w[ clean clean_shared incremental ]

task :default => %w[ clean clean_shared shared pic12 clean pic16 ]