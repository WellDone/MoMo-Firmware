OUT_FILE = "build/test/out.txt"
File.delete OUT_FILE if File.exists? OUT_FILE 
IO.popen("sim30 test/simulation/sim_instructions.txt")
sleep 1
if File.exists? OUT_FILE 
    file_contents = File.read OUT_FILE
    print file_contents
end
