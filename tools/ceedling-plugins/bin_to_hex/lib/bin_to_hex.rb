require 'plugin'
require 'constants'

class BinToHex < Plugin
  def post_link_execute( arg_hash )
    print "Generating .hex file from .elf binary...\n"
    sh "xc16-bin2hex #{PROJECT_RELEASE_BUILD_TARGET}"
    print "Build Successful!\n"
  end
end