import re
import glob
import sys
import os
import subprocess
import threading
import time

runs_each = 50

if len(sys.argv) < 2:
   print("Expected path to binary")
   exit(1)

binary = sys.argv[1]

print("\n<< BENCH >>\n")
print("Binary: ", binary)
print("N Executions per benchmark: ", runs_each)
check_directory = os.getcwd()
print("CWD : ", check_directory)

print("\n------- BEGIN BENCHMARK -------\n")

def test_item(id, expected_result, item):

   print("Running:", item["path"])
   print("Expecting return code: ", expected_result)

   cum_time = 0
   for x in range(0, runs_each):
      start = time.time()
      result = subprocess.run([binary, item["path"]], stdout=subprocess.PIPE)
      end = time.time()

      cum_time += end - start

      if result.returncode != int(expected_result):
         print("FAIL: Incorrect return code received: ", result.returncode)
         exit(1)

   avg = cum_time / runs_each
   sec_avg = str(round(avg, 4)) + "s"
   ms_avg = str(round(avg * 1000, 4)) + "ms"
   item["sec"] = sec_avg
   item["ms"] = ms_avg

   print("Average execution time: ", sec_avg, "(", ms_avg, ")\n")
   return item

def retrieve_objects_from(directory):
   os.chdir(directory)
   items_in_dir = glob.glob("*.nibi")
   results = []
   for item in items_in_dir:
      result = {}
      result["name"] = item
      result["path"] = directory + "/" + item
      results.append(result)
   return results

def generate_table(results):
   print('''
| test            | time (s)  | time (ms)
|----             |----       |----''')
   for item in results:
     print('|', item["name"], '|', item["sec"], '|', item["ms"], '|')

def task(id, jobs):
   results = []
   for item in jobs:
      results.append(test_item(id, 0, item))
   return results

def run():
   exec_list = retrieve_objects_from(check_directory)
   os.chdir(check_directory)
   generate_table(task(0, exec_list))

run_time_start = time.time()
run()
run_time_end = time.time()

total_time = run_time_end - run_time_start
secs_ran = str(round(total_time, 4)) 
mins_ran = str(round((total_time / 60), 4))

print("\n-------- END BENCHMARK --------\n")
print("Total time was ", secs_ran, " seconds (", mins_ran, " minutes)")
