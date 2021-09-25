MRuby::Build.new do |conf|
  toolchain :gcc
  conf.gembox 'default'
  conf.gem File.expand_path(File.dirname(__FILE__)) do |g|
    g.download_libgit2
  end
  conf.enable_test

  if ENV['DEBUG'] == 'true'
    conf.enable_debug
    conf.cc.defines = %w(MRB_ENABLE_DEBUG_HOOK)
    conf.gem core: 'mruby-bin-debugger'
  end
end
