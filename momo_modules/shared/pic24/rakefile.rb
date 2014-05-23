PROJECT_CEEDLING_ROOT = "../../../tools/ceedling"
load "#{PROJECT_CEEDLING_ROOT}/lib/rakefile.rb"

task :buildFJ do
	TARGET_EXT = "fj"
	PROC = "24FJ64GA306"
	PROC_DEFINE = "__PIC24FJ64GA306__"
	Rake::Task["clean"].invoke
	Rake::Task["release"].invoke
end

task :buildF do
	TARGET_EXT = "f"
	PROC = "24F16KA101"
	PROC_DEFINE = "__PIC24F16KA101__"
	Rake::Task["clean"].invoke
	Rake::Task["release"].invoke
end