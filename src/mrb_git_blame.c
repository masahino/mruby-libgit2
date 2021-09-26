/*
** mrb_git_blame.c - Git::Blame class
**
** Copyright (c) masahino 2021
**
** See Copyright Notice in LICENSE
*/

#include "mruby.h"
#include "mruby/class.h"
#include "mruby/data.h"
#include "mruby/hash.h"
#include "mruby/string.h"
#include "git2.h"

static void mrb_git_blame_free(mrb_state *mrb, void *ptr) {
  if (ptr != NULL) {
    git_blame_free((git_blame *)ptr);
  }
}

const struct mrb_data_type mrb_git_blame_data_type = {
  "mrb_git_blame_data", mrb_git_blame_free,
};

static mrb_value
mrb_git_oid_to_s(mrb_state *mrb, const git_oid *oid)
{
  char out[40];
  git_oid_fmt(out, oid);
  return mrb_str_new_cstr(mrb, out);
}

static mrb_value
mrb_git_signature_to_h(mrb_state *mrb, const git_signature *sig)
{
  mrb_value sig_h = mrb_hash_new(mrb);
  mrb_value time_obj;
  time_obj = mrb_funcall(mrb, mrb_obj_value(mrb_class_get(mrb, "Time")),
    "at", 1, mrb_fixnum_value(sig->when.time));
  time_obj = mrb_funcall(mrb, time_obj, "getlocal", 1, mrb_fixnum_value(sig->when.offset * 60));
  mrb_hash_set(mrb, sig_h, mrb_symbol_value(mrb_intern_cstr(mrb, "name")),
    mrb_str_new_cstr(mrb, sig->name));
  mrb_hash_set(mrb, sig_h, mrb_symbol_value(mrb_intern_cstr(mrb, "email")),
    mrb_str_new_cstr(mrb, sig->email));
  mrb_hash_set(mrb, sig_h, mrb_symbol_value(mrb_intern_cstr(mrb, "time")), time_obj);
  return sig_h;
}

static mrb_value
mrb_git_hunk_to_hash(mrb_state *mrb, const git_blame_hunk *hunk)
{
  mrb_value hunk_h = mrb_hash_new(mrb);
  mrb_hash_set(mrb, hunk_h, mrb_symbol_value(mrb_intern_cstr(mrb, "lines_in_hunk")),
    mrb_fixnum_value(hunk->lines_in_hunk));
  mrb_hash_set(mrb, hunk_h, mrb_symbol_value(mrb_intern_cstr(mrb, "final_commit_id")),
    mrb_git_oid_to_s(mrb, &(hunk->final_commit_id)));
  mrb_hash_set(mrb, hunk_h, mrb_symbol_value(mrb_intern_cstr(mrb, "final_start_line_number")),
    mrb_fixnum_value(hunk->final_start_line_number));
  mrb_hash_set(mrb, hunk_h, mrb_symbol_value(mrb_intern_cstr(mrb, "final_signature")),
    hunk->final_signature ? mrb_git_signature_to_h(mrb, hunk->final_signature) : mrb_nil_value());
  mrb_hash_set(mrb, hunk_h, mrb_symbol_value(mrb_intern_cstr(mrb, "orig_commit_id")),
    mrb_git_oid_to_s(mrb, &(hunk->orig_commit_id)));
    //mrb_fixnum_value(hunk->orig_commit_id));
  mrb_hash_set(mrb, hunk_h, mrb_symbol_value(mrb_intern_cstr(mrb, "orig_path")),
    mrb_str_new_cstr(mrb, hunk->orig_path));
  mrb_hash_set(mrb, hunk_h, mrb_symbol_value(mrb_intern_cstr(mrb, "orig_start_line_number")),
    mrb_fixnum_value(hunk->orig_start_line_number));
  mrb_hash_set(mrb, hunk_h, mrb_symbol_value(mrb_intern_cstr(mrb, "orig_sitnature")),
    hunk->orig_signature ? mrb_git_signature_to_h(mrb, hunk->orig_signature) : mrb_nil_value());
  mrb_hash_set(mrb, hunk_h, mrb_symbol_value(mrb_intern_cstr(mrb, "boundary")),
    hunk->boundary ? mrb_true_value() : mrb_false_value());
  
  return hunk_h;
}

static mrb_value
mrb_git_blame_initialize(mrb_state *mrb, mrb_value self)
{
  mrb_value repo_obj, options_obj;
  git_repository *repo;
  const char *path;
  git_blame_options opts = GIT_BLAME_OPTIONS_INIT;
  git_blame *blame;
  int ret;

  DATA_TYPE(self) = &mrb_git_blame_data_type;
  DATA_PTR(self) = NULL;
  
  mrb_get_args(mrb, "oz|o", &repo_obj, &path, &options_obj);
  repo = (git_repository *)DATA_PTR(repo_obj);
  ret = git_blame_file(&blame, repo, path, &opts);
  if (ret < 0) {
    mrb_raisef(mrb, E_RUNTIME_ERROR, "error %d", ret);
  }
  DATA_PTR(self) = blame;
  return self;
}

static mrb_value
mrb_git_blame_get_hunk_count(mrb_state *mrb, mrb_value self)
{
  git_blame *blame = (git_blame *)DATA_PTR(self);
  int count;
  count = git_blame_get_hunk_count(blame);
  if (count < 0) {
    mrb_raisef(mrb, E_RUNTIME_ERROR, "error %d", count);
  }

  return mrb_fixnum_value(count);
}

static mrb_value
mrb_git_blame_get_hunk_byline(mrb_state *mrb, mrb_value self)
{
  git_blame *blame = (git_blame *)DATA_PTR(self);
  const git_blame_hunk *hunk;
  mrb_value hunk_hash;
  int line;
  mrb_get_args(mrb, "i", &line);
  hunk = git_blame_get_hunk_byline(blame, line);
  if (hunk == NULL) {
    mrb_raise(mrb, E_RUNTIME_ERROR, "error");
  }
  hunk_hash = mrb_git_hunk_to_hash(mrb, hunk);
  return hunk_hash;
}

void
mrb_mruby_git_blame_init(mrb_state *mrb, struct RClass *git)
{
  struct RClass *blame;
  blame = mrb_define_class_under(mrb, git, "Blame", mrb->object_class);
  MRB_SET_INSTANCE_TT(blame, MRB_TT_DATA);

  mrb_define_method(mrb, blame, "initialize", mrb_git_blame_initialize, MRB_ARGS_ARG(2, 1));
  mrb_define_method(mrb, blame, "hunk_count", mrb_git_blame_get_hunk_count, MRB_ARGS_NONE());
  mrb_define_method(mrb, blame, "hunk_byline", mrb_git_blame_get_hunk_byline, MRB_ARGS_REQ(1));


}
