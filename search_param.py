import optuna
import os
import pwn
import sys

pwn.context.log_level = "error"

C = int(sys.argv[1])

params = []
for l in open("seeds.txt"):
  params += [list(map(int, l.split()))]

def objective(trial):
  level_start = trial.suggest_int("level_start", 2, 16)
  level_end = trial.suggest_int("level_end", 2, 16)
  prospect_p = trial.suggest_int("prospect_p", 1, 256)
  prospect_n = trial.suggest_int("prospect_n", 1, 20)
  break_decay = trial.suggest_int("break_decay", 1, 99)
  break_add = trial.suggest_int("break_add", 1, 100)

  score = 0
  n = 0
  for i in range(len(params)):
    if params[i][3]==C:
      s = pwn.process(f"./A {level_start} {level_end} {prospect_p} {prospect_n} {break_decay} {break_add} < in/{i:04d}.txt > /dev/null", shell=True)
      score += int(s.readline().split()[-1])
      n += 1
      s.close()
  return score/n

study = optuna.create_study(
  study_name=f"ahc018_5_{C}",
  storage="sqlite:///db.sqlite3",
  load_if_exists=True,
  direction="minimize")
study.optimize(objective)
