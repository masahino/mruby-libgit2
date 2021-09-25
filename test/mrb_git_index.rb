##
## Git::Index Test
##

def setup
  Git.libgit2_init
  tmpdir = File.dirname(__FILE__)+'/tmp/'
  `mkdir -p #{tmpdir}`
  @tmppath = Dir.mktmpdir(nil, tmpdir)
end

def teardown
  Git.libgit2_shutdown
end

assert("Git::Index.path") do
  setup
  repo = Git::Repository.new(@tmppath, true)
  index = repo.index
  assert_kind_of Git::Index, index
  assert_equal @tmppath+'/index', index.path
  teardown
end
