##
## Libgit2 Test
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

assert("Git::Repository.new : without Git.libgit2_init") do
  tmpdir = File.dirname(__FILE__)+'/tmp/'
  `mkdir -p #{tmpdir}`
  tmppath = Dir.mktmpdir(nil, tmpdir)
  repo = Git::Repository.new(tmppath, true)
  assert_kind_of Git::Repository, repo
  assert_equal 1, Git.libgit2_init_count
  Git.libgit2_shutdown
end

assert("Git::Repository.new is_bare = true") do
  setup
  repo = Git::Repository.new(@tmppath, true)
  assert_kind_of Git::Repository, repo
  assert_equal true, repo.bare?
  teardown
end

assert("Git::Repository.new is_bare = false") do
  setup
  repo = Git::Repository.new(@tmppath, false)
  assert_kind_of Git::Repository, repo
  assert_equal false, repo.bare?
  teardown
end

assert("Git::Repository.open") do
  setup
  repo1 = Git::Repository.new(@tmppath, false)
  repo2 = Git::Repository.open(@tmppath)
  assert_equal @tmppath + "/.git/", repo2.path
  teardown
end

assert("Git::Repository.open : without Git.libgit2_init") do
  tmpdir = File.dirname(__FILE__)+'/tmp/'
  `mkdir -p #{tmpdir}`
  tmppath = Dir.mktmpdir(nil, tmpdir)
  Git::Repository.new(tmppath, false)
  Git.libgit2_shutdown
  repo = Git::Repository.open(tmppath)
  assert_kind_of Git::Repository, repo
  assert_equal 1, Git.libgit2_init_count
  Git.libgit2_shutdown
end

assert("Git::Repository.discover") do
  repo = Git::Repository.new(@tmppath, false)
  path = Git::Repository.discover(@tmppath)
  assert_equal @tmppath + "/.git/", path
  teardown
end
