/*
** mrb_libgit2.c - Libgit2 class
**
** Copyright (c) masahino 2021
**
** See Copyright Notice in LICENSE
*/

#include "mruby.h"
#include "mruby/class.h"
#include "mruby/data.h"
#include "mruby/string.h"
#include "mruby/variable.h"
#include "git2.h"

extern const struct mrb_data_type mrb_git_index_data_type;

static void mrb_git_repository_free(mrb_state *mrb, void *ptr) {
  if (ptr != NULL) {
    git_repository_free((git_repository *)ptr);
  }
}

static const struct mrb_data_type mrb_git_repository_data_type = {
  "mrb_git_repository_data", mrb_git_repository_free,
};

static mrb_value
mrb_git_repository_init(mrb_state *mrb, mrb_value self)
{
  git_repository *repository;
  const char *path;
  mrb_value init_count;
  mrb_bool is_bare = FALSE;
  int ret;

  DATA_TYPE(self) = &mrb_git_repository_data_type;
  DATA_PTR(self) = NULL;

  mrb_get_args(mrb, "zb", &path, &is_bare);
  /* check init count */
  init_count = mrb_mod_cv_get(mrb, mrb_module_get(mrb, "Git"), mrb_intern_cstr(mrb, "@@git_init_count"));
  if (mrb_integer(init_count) == 0) {
    mrb_funcall(mrb, mrb_obj_value(mrb_module_get(mrb, "Git")), "libgit2_init", 0);
  }
  ret = git_repository_init(&repository, path, is_bare);
  if (ret < 0) {
    mrb_raisef(mrb, E_RUNTIME_ERROR, "error %d", ret);
  }
  DATA_PTR(self) = repository;

  return self;
}

static mrb_value
mrb_git_repository_open(mrb_state *mrb, mrb_value self)
{
  git_repository *repository;
  const char *path;
  struct RClass *repo_klass = mrb_class_get_under(mrb, mrb_module_get(mrb, "Git"), "Repository");
  mrb_value init_count;
  int ret;
  mrb_value repo_obj = mrb_obj_value(Data_Wrap_Struct(mrb, repo_klass, &mrb_git_repository_data_type, NULL));
  DATA_TYPE(repo_obj) = &mrb_git_repository_data_type;
  DATA_PTR(repo_obj) = NULL;

  mrb_get_args(mrb, "z", &path);
  /* check init count */
  init_count = mrb_mod_cv_get(mrb, mrb_module_get(mrb, "Git"), mrb_intern_cstr(mrb, "@@git_init_count"));
  if (mrb_integer(init_count) == 0) {
    mrb_funcall(mrb, mrb_obj_value(mrb_module_get(mrb, "Git")), "libgit2_init", 0);
  }
  ret = git_repository_open(&repository, path);

  if (ret < 0) {
    mrb_raisef(mrb, E_RUNTIME_ERROR, "error %d", ret);
  }
  DATA_PTR(repo_obj) = repository;

  return repo_obj;
}

static mrb_value
mrb_git_repository_discover(mrb_state *mrb, mrb_value self)
{
  const char *start_path;
  mrb_value across_fs_obj;
  mrb_int argc;
  int across_fs = 0;
  int ret;
  git_buf repository_path = { NULL };

  argc = mrb_get_args(mrb, "z|b", &start_path, &across_fs_obj);
  across_fs = mrb_bool(across_fs_obj);
  ret = git_repository_discover(&repository_path, start_path, across_fs, NULL);
  if (ret < 0) {
    mrb_raisef(mrb, E_RUNTIME_ERROR, "error %d", ret);
  }
  if (repository_path.ptr != NULL) {
    return mrb_str_new_cstr(mrb, repository_path.ptr);
  }
  return mrb_nil_value();
}

static mrb_value
mrb_git_repository_is_bare(mrb_state *mrb, mrb_value self)
{
  git_repository *repo = (git_repository *)DATA_PTR(self);
  int is_bare;
  is_bare = git_repository_is_bare(repo);
  return mrb_bool_value(is_bare);
}

static mrb_value
mrb_git_repository_get_index(mrb_state *mrb, mrb_value self)
{
  git_repository *repo = (git_repository *)DATA_PTR(self);
  git_index *index;

  struct RClass *index_class = mrb_class_get_under(mrb, mrb_module_get(mrb, "Git"), "Index");
  int ret;
  mrb_value index_obj = mrb_obj_value(Data_Wrap_Struct(mrb, index_class, &mrb_git_index_data_type, NULL));
  DATA_TYPE(index_obj) = &mrb_git_index_data_type;
  DATA_PTR(index_obj) = NULL;

  ret = git_repository_index(&index, repo);

  if (ret < 0) {
    mrb_raisef(mrb, E_RUNTIME_ERROR, "error %d", ret);
  }
  DATA_PTR(index_obj) = index;

  return index_obj;
}

static mrb_value
mrb_git_repository_path(mrb_state *mrb, mrb_value self)
{
  git_repository *repo = (git_repository *)DATA_PTR(self);
  const char *path;
  path = git_repository_path(repo);
  return mrb_str_new_cstr(mrb, path);
}

void mrb_mruby_git_repository_init(mrb_state *mrb, struct RClass *git)
{
  struct RClass *repository;
  repository = mrb_define_class_under(mrb, git, "Repository", mrb->object_class);
  MRB_SET_INSTANCE_TT(repository, MRB_TT_DATA);

  mrb_define_class_method(mrb, repository, "open", mrb_git_repository_open, MRB_ARGS_REQ(1));
  mrb_define_class_method(mrb, repository, "discover", mrb_git_repository_discover, MRB_ARGS_ARG(1, 1));
  mrb_define_method(mrb, repository, "initialize", mrb_git_repository_init, MRB_ARGS_REQ(2));
  mrb_define_method(mrb, repository, "bare?", mrb_git_repository_is_bare, MRB_ARGS_NONE());
  mrb_define_method(mrb, repository, "index", mrb_git_repository_get_index, MRB_ARGS_NONE());
  mrb_define_method(mrb, repository, "path", mrb_git_repository_path, MRB_ARGS_NONE());
}
