assert('Git.clone') do 
  Git.libgit2_init
  url = 'file://' + File.dirname(__FILE__) + '/test_repo/'
  local_path = File.dirname(__FILE__) + '/clone_test/'
  if Dir.exist? local_path
    `rm -rf #{local_path}`
  end
  repo = Git.clone(url, local_path)
  assert_kind_of Git::Repository, repo
  assert_equal local_path + '.git/', repo.path
  `rm -rf #{local_path}`
  Git.libgit2_shutdown
end