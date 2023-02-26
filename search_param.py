import optuna
import os
import pwn
import sys

pwn.context.log_level = "error"

C = int(sys.argv[1])
#K = int(sys.argv[1])

params = []
for l in open("seeds.txt"):
  params += [list(map(int, l.split()))]

def objective(trial):
  level_start = trial.suggest_int("level_start", 11, 18)
  level_end = trial.suggest_int("level_end", 6, 9)
  prospect_p = trial.suggest_int("prospect_p", 1, 256)
  prospect_n = trial.suggest_int("prospect_n", 1, 20)
  break_decay = trial.suggest_int("break_decay", 50, 99)
  break_add = trial.suggest_int("break_add", 1, 50)
  dijkstra_c = trial.suggest_int("dijkstra_c", 0, 256)

  score = 0
  n = 0
  for i in range(len(params)):
    if params[i][3]==C:
    #if params[i][2]==K:
      s = pwn.process(f"./A {level_start} {level_end} {prospect_p} {prospect_n} {break_decay} {break_add} {dijkstra_c} < in/{i:04d}.txt > /dev/null", shell=True)
      score += int(s.readline().split()[-1])
      n += 1
      s.close()
  return score/n

study = optuna.create_study(
  study_name=f"ahc018_15_{C}",
  storage="sqlite:////home/kusano/ahc018/db.sqlite3",
  load_if_exists=True,
  direction="minimize")
study.optimize(objective)
