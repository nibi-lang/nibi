#!/usr/bin/env python3

'''
  This script is a do-it-all script for building and installing Nibi and its modules.
  It also runs tests for Nibi, its modules, and it can run the benchmark / performance tests.
'''

import os
import shutil
import subprocess
import argparse

parser = argparse.ArgumentParser()
parser.add_argument("-n", "--install_nibi", action="store_true", help="Builds and installs Nibi library and application")
parser.add_argument("-m", "--install_modules", action="store_true", help="Builds and installs Nibi modules")
parser.add_argument("-d", "--debug", action="store_true", help="Builds Nibi and modules in debug mode")
parser.add_argument("-t", "--test", action="store_true", help="Runs all tests for Nibi")
parser.add_argument("-p", "--perf", action="store_true", help="Runs performance tests for Nibi")
parser.add_argument("-c", "--check_modules", action="store_true", help="Details all modules installed, and runs their tests")
parser.add_argument("-a", "--all_the_things", action="store_true", help="Installs Nibi, modules, runs tests, and runs performance tests")
parser.add_argument("-s", "--scrub", action="store_true", help="Scrub the system of all Nibi and modules and build files")
args = parser.parse_args()

if not any(vars(args).values()):
  print("No arguments provided.\n")
  parser.print_help()
  exit(0)

cwd = os.getcwd()

build_type = "-DCMAKE_BUILD_TYPE=Release"

if args.debug:
  build_type = "-DCMAKE_BUILD_TYPE=Debug"

NIBI_PATH = os.environ.get('NIBI_PATH')
if NIBI_PATH is None:
  print("Please set NIBI_PATH to the path where you want to install NIBI")
  exit(1)

print("NIBI_PATH is set to: " + NIBI_PATH)

if not os.path.exists(NIBI_PATH):
  print("NIBI_PATH does not exist! Please create it and try again.")
  exit(1)

def program_exists(cmd):
  result = subprocess.run(["which", cmd], stdout=subprocess.PIPE)
  return result.returncode == 0

def ensure_nibi_installed():
  if not program_exists("nibi"):
    print("Nibi is not installed yet. Please install it with -n and try again.")
    exit(1)

def execute_command(cmd):
  result = subprocess.run(cmd, stdout=subprocess.PIPE)
  if result.returncode != 0:
    print("\nCommand failed: " + str(cmd) + ". Output:\n " + result.stdout.decode("utf-8"))
    exit(1)
  return result.stdout.decode("utf-8")

def scrub():
  os.chdir("./nibi")
  if os.path.exists("./build"):
    shutil.rmtree("./build")
  target_dest = NIBI_PATH + "/modules"
  if os.path.exists(target_dest):
    shutil.rmtree(target_dest)

def build_and_install_nibi():
  print("Building and installing Nibi library and application")
  os.chdir("./nibi")

  if not os.path.exists("./build"):
    os.mkdir("./build")

  os.chdir("./build")
  execute_command(["cmake", "..", build_type])
  execute_command(["make", "-j4"])
  execute_command(["sudo", "make", "install"])
  print("SUCCESS")
  os.chdir(cwd)

def build_current_module(module):

  library_name = module + ".lib"

  print("Building library: " + library_name)

  if os.path.exists("./" + library_name):
    os.remove("./" + library_name)

  if not os.path.exists("./build"):
    os.mkdir("./build")

  os.chdir("./build")
  execute_command(["cmake", "..", build_type])
  execute_command(["make", "-j4"])

  if not os.path.exists(module + ".lib"):
    print("No library found for module: " + module)
    exit(1)

  shutil.copyfile(library_name, "../" + library_name)
  os.chdir("../")
  shutil.rmtree("./build")

def install_module(module):
  module_dir = os.getcwd()
  os.chdir(module)
  
  target_dest = NIBI_PATH + "/modules/" + module

  if os.path.exists(target_dest):
    print("Updating existing module: " + module)
    shutil.rmtree(target_dest)

  if not os.path.exists(NIBI_PATH + "/modules"):
    os.mkdir(NIBI_PATH + "/modules")

  if not os.path.exists(target_dest):
    os.mkdir(target_dest)

  print("Installing module to : " + target_dest)

  if os.path.exists("CMakeLists.txt"):
    print("Found CMakeLists.txt in module: " + module)
    build_current_module(module)

  files = os.listdir(".")
  for file in files:
    if file.endswith(".nibi") or file.endswith(".lib"):
      print("Copying file: " + file)
      shutil.copyfile(file, target_dest + "/" + file)

    if os.path.isdir(file) and file == "tests":
      print("Copying tests: " + file)
      shutil.copytree(file, target_dest + "/" + file)

  os.chdir(module_dir)

def build_and_install_modules():
  os.chdir("./nibi/modules")
  for module in os.listdir("."):
    print("Building and installing module: " + module)
    install_module(module)
    print("\n")
  os.chdir(cwd)

def setup_tests():
  # One of the tests requires a module to be built, but not installed
  os.chdir("./test_scripts/tests/module")
  build_current_module("module")
  os.chdir(cwd)

def run_tests():
  os.chdir("./test_scripts")
  print("Running tests...")
  execute_command(["python3", "run.py", "nibi"])
  print("SUCCESS")
  os.chdir(cwd)

def run_perfs():
  os.chdir("./test_perfs")
  print("Running performance tests... this may take a couple of minutes...")
  result = execute_command(["python3", "run.py", "nibi"])
  print(result)
  print("SUCCESS")
  os.chdir(cwd)

def check_modules():
  os.chdir(NIBI_PATH)
  if not os.path.exists("./modules"):
    print("No modules to check. Exiting.")
    exit(0)

  os.chdir("./modules")

  modules = os.listdir("./")
  print("\nFound ", len(modules), " installed modules.\n")

  for module in os.listdir("./"):
    print("Checking module: " + module)
    print(execute_command(["nibi", "-m", module]))
    print("\n<MODULE TESTS>\n")
    print(execute_command(["nibi", "-t", module]))
    print("\n------------\n")

  os.chdir(cwd)

if not program_exists("make"):
  print("make is not installed. Please install it and try again.")
  exit(1)

if not program_exists("cmake"):
  print("CMake is not installed. Please install it and try again.")
  exit(1)

if args.install_nibi or args.all_the_things:
  build_and_install_nibi()

if args.install_modules or args.all_the_things:
  if not os.path.exists("./nibi/modules"):
    print("No modules found. Exiting.")
    exit(0)
  print("\n")
  build_and_install_modules()

if args.test or args.all_the_things:
  ensure_nibi_installed()

  # Ensure tests are setup
  setup_tests()

  # Run tests
  run_tests()

if args.check_modules or args.all_the_things:
  ensure_nibi_installed()
  check_modules()

if args.perf or args.all_the_things:
  ensure_nibi_installed()

  run_perfs()

if args.scrub:
  ensure_nibi_installed()

  scrub()
