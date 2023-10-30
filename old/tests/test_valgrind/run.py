import re
import glob
import sys
import os
import subprocess
import threading
import time

if len(sys.argv) < 2:
   print("Expected path to binary")
   exit(1)

binary = sys.argv[1]

valgrind_command = "valgrind"
valgrind_arg = "--leak-check=full"

print("\n<< BENCH >>\n")
print("Binary: ", binary)
check_directory = os.getcwd()
print("CWD : ", check_directory)

print("\n------- BEGIN VALGRIND -------\n")

def test_item(id, expected_result, item):

   print("Running:", item["path"])
   print("Expecting return code: ", expected_result)

   result = subprocess.run(
           [valgrind_command, valgrind_arg, binary, item["path"]],
           capture_output=True, text=True)

   if result.returncode != int(expected_result):
      print("FAIL: Incorrect return code received: ", result.returncode)
      exit(1)

   output = result.stderr
   print(output)

   if "HEAP SUMMARY" not in output:
      print("FAIL: Suspected valgrind failure (did it launch?)")
      exit(1)

   if "no leaks are possible" not in output:
      print("FAIL: Memory leak detected:")
      print(output.decode("utf-8"))
      exit(1)

   print("TEST COMPLETE")
   return

def retrieve_objects_from(directory):
   os.chdir(directory)
   items_in_dir = glob.glob("*.nibi")
   results = []
   for item in items_in_dir:
      result = {}
      result["name"] = item
      result["path"] = directory + "/" + item
      results.append(result)
   print("Targets: ", results)
   return results

def task(id, jobs):
   results = []
   for item in jobs:
      results.append(test_item(id, 0, item))
   return results

def run():
   exec_list = retrieve_objects_from(check_directory)
   os.chdir(check_directory)
   task(0, exec_list)

run_time_start = time.time()
run()
run_time_end = time.time()

total_time = run_time_end - run_time_start
secs_ran = str(round(total_time, 4)) 
mins_ran = str(round((total_time / 60), 4))

print("Total time was ", secs_ran, " seconds (", mins_ran, " minutes)")
