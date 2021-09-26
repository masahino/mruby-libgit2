assert('Git::Blame tests setup') do
  ret = Git.libgit2_init
  url = 'file://' + File.dirname(__FILE__) + '/test_repo/'
  @local_path = File.dirname(__FILE__) + '/index_test/'
  if Dir.exist? @local_path
    `rm -rf #{@local_path}`
  end
  @repo = Git.clone(url, @local_path)
  assert_equal 1, ret
end

assert('Git::Blame.new') do
  blame = Git::Blame.new(@repo, 'test.txt')
  assert_kind_of Git::Blame, blame
end

assert('Git::Blame#hunk_count') do
  blame = Git::Blame.new(@repo, 'test.txt')
  assert_equal 4, blame.hunk_count
end

assert('Git::Blame#hunk_byline') do
  blame = Git::Blame.new(@repo, 'test.txt')
  hunk = blame.hunk_byline(1)
  assert_kind_of Hash, hunk
  assert_equal 1, hunk[:lines_in_hunk]
  assert_kind_of String, hunk[:final_commit_id]
  assert_equal 1, hunk[:final_start_line_number]
  assert_kind_of Hash, hunk[:final_signature]
  assert_kind_of String, hunk[:final_signature][:name]
  assert_kind_of Time, hunk[:final_signature][:time]
  assert_kind_of String, hunk[:orig_commit_id]
  assert_equal 'test.txt', hunk[:orig_path]
  assert_equal 1, hunk[:orig_start_line_number]
  assert_kind_of nil, hunk[:orig_signature]
  assert_equal false, hunk[:boundary]
end

assert('Git::Blame tests teardown') do
  ret = Git.libgit2_shutdown
  assert_equal 0, ret
end
