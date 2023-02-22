import optuna
import os
import pwn
import sys

pwn.context.log_level = "error"

def objective(trial):
  level_start = trial.suggest_int("level_start", 2, 14)
  level_end = trial.suggest_int("level_end", 2, 14)
  prospect_p = trial.suggest_int("prospect_p", 1, 1000)
  prospect_n = trial.suggest_int("prospect_n", 1, 100)
  break_decay = trial.suggest_int("break_decay", 1, 99)
  break_add = trial.suggest_int("break_add", 1, 1000)

  score = 0
  for i in range(100):
    s = pwn.process(f"./A {level_start} {level_end} {prospect_p} {prospect_n} {break_decay} {break_add} < tools/in/{i:04d}.txt > /dev/null", shell=True)
    score += int(s.readline().split()[-1])
    s.close()
  return score

study = optuna.create_study(
  study_name="ahc018_1",
  storage="sqlite:///db.sqlite3",
  load_if_exists=True,
  direction="minimize")
study.optimize(objective)
