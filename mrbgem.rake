MRuby::Gem::Specification.new('mruby-libgit2') do |spec|
  
  spec.license = 'MIT'
  spec.authors = 'masahino'
  spec.add_test_dependency 'mruby-tempfile'

  def spec.download_libgit2
    return if @libgit2_downloaded
    @libgit2_downloaded = true

    require 'open-uri'
    libgit2_ver = "1.2.0"
    libgit2_url = "https://github.com/libgit2/libgit2/archive/refs/tags/v#{libgit2_ver}.tar.gz"
    libgit2_build_root = "#{build_dir}/libgit2/"
    libgit2_dir = "#{libgit2_build_root}/libgit2-#{libgit2_ver}"
    libgit2_build = "#{libgit2_build_root}/libgit2-#{libgit2_ver}/build"
    libgit2_a = "#{libgit2_build}/libgit2.a"
    git2_h = "#{libgit2_dir}/include/git2.h"
    libmruby_a = libfile("#{build.build_dir}/lib/libmruby")

    task :clean do
      if File.exists?(git2_h)
        Dir.chdir(libgit2_build) do
          `make clean`
        end
      end
    end

    file git2_h do |t|
      URI.open(libgit2_url) do |http|
        tar = http.read
        FileUtils.mkdir_p libgit2_build_root
        IO.popen("tar xfz - -C #{filename libgit2_build_root}", "wb") do |f|
          f.write tar
        end
      end
    end

    def run_command(env, command)
      unless system(env, command)
        fail "#{command} failed"
      end
    end

    file libgit2_a => git2_h do |t|
      FileUtils.mkdir_p libgit2_build
      Dir.chdir(libgit2_build) do
        e = {
          'CC' => "#{build.cc.command} #{build.cc.flags.join(' ')}",
          'CXX' => "#{build.cxx.command} #{build.cxx.flags.join(' ')}",
          'LD' => "#{build.linker.command} #{build.linker.flags.join(' ')}",
          'AR' => build.archiver.command }
#        run_command(e, "cmake .. -DBUILD_SHARED_LIBS=OFF -DUSE_BUNDLED_ZLIB=ON -DUSE_SSH=OFF -DUSE_ICONV=OFF")
        run_command(e, "cmake .. -DBUILD_SHARED_LIBS=OFF -DUSE_BUNDLED_ZLIB=ON")
        run_command(e, "cmake --build .")
      end     
    end

    file libmruby_a => libgit2_a
    srcs = Dir.glob("#{dir}/src/*.c")
    srcs.each{|src| file src => libgit2_a }
    self.linker.library_paths << libgit2_build
    self.linker.libraries << 'git2'
    if RUBY_PLATFORM =~ /darwin/
      self.linker.flags_after_libraries += ['-framework Security', '-framework CoreFoundation']
      self.linker.libraries << 'iconv'
    end
    [self.cc, self.cxx, self.objc, self.mruby.cc, self.mruby.cxx, self.mruby.objc].each do |cc|
      cc.include_paths << libgit2_dir+"/include"
    end
  end
end
