##
## Git::Index Test
##

def setup
  Git.libgit2_init
  url = 'file://' + File.dirname(__FILE__) + '/test_repo/'
  @local_path = File.dirname(__FILE__) + '/index_test/'
  if Dir.exist? @local_path
    `rm -rf #{@local_path}`
  end
  @repo = Git.clone(url, @local_path)
end

def teardown
  `rm -rf #{@local_path}`
  Git.libgit2_shutdown
end

assert('setup') do
  setup
  assert_equal 1, Git.libgit2_init_count
end

assert("Git::Index#add #bypath") do
  new_file_path = @local_path + 'new_file'
  File.open(new_file_path, 'w') do |f|
    f.puts "add new file"
  end
  index = @repo.index
  ret = index.add('new_file')
  assert_equal 0, ret
end

assert("Git::Index#path") do
  index = @repo.index
  assert_kind_of Git::Index, index
  assert_equal @local_path + '.git/index', index.path
end

assert('teardown') do
  teardown
  assert_equal 0, Git.libgit2_init_count
end