/*
** mrb_git_clone.c - Git.clone
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

extern const struct mrb_data_type mrb_git_repository_data_type;

static mrb_value
mrb_git_clone(mrb_state *mrb, mrb_value self)
{
  git_repository *repo;
  const char *url, *local_path;
  struct RClass *repo_klass = mrb_class_get_under(mrb, mrb_module_get(mrb, "Git"), "Repository");
  mrb_value init_count;
  mrb_value options_obj;
  int ret;
  git_clone_options options = GIT_CLONE_OPTIONS_INIT;

  mrb_value repo_obj = mrb_obj_value(Data_Wrap_Struct(mrb, repo_klass, &mrb_git_repository_data_type, NULL));
  DATA_TYPE(repo_obj) = &mrb_git_repository_data_type;
  DATA_PTR(repo_obj) = NULL;

  mrb_get_args(mrb, "zz|o", &url, &local_path, &options_obj);
  /* check init count */
  init_count = mrb_mod_cv_get(mrb, mrb_module_get(mrb, "Git"), mrb_intern_cstr(mrb, "@@git_init_count"));
  if (mrb_integer(init_count) == 0) {
    mrb_funcall(mrb, mrb_obj_value(mrb_module_get(mrb, "Git")), "libgit2_init", 0);
  }
  ret = git_clone(&repo, url, local_path, &options);

  if (ret < 0) {
    mrb_raisef(mrb, E_RUNTIME_ERROR, "error %d", ret);
  }
  DATA_PTR(repo_obj) = repo;

  return repo_obj;
}

void
mrb_mruby_git_clone_init(mrb_state *mrb, struct RClass *git)
{
  mrb_define_module_function(mrb, git, "clone", mrb_git_clone, MRB_ARGS_ARG(2, 1));
}
