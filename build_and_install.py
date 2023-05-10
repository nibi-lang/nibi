import os
import shutil
import subprocess
import argparse

parser = argparse.ArgumentParser()
parser.add_argument("-n", "--install_nibi", action="store_true")
parser.add_argument("-m", "--install_modules", action="store_true")
args = parser.parse_args()

cwd = os.getcwd()

NIBI_PATH = os.environ.get('NIBI_PATH')
if NIBI_PATH is None:
  print("Please set NIBI_PATH to the path where you want to install NIBI")
  exit(1)

print("NIBI_PATH is set to: " + NIBI_PATH)

if not os.path.exists(NIBI_PATH):
  print("NIBI_PATH does not exist! Please create it and try again.")
  exit(1)

def execute_command(cmd):
  result = subprocess.run(cmd, stdout=subprocess.PIPE)
  if result.returncode != 0:
    print("Command failed: " + str(cmd) + ". Output:\n " + result.stdout.decode("utf-8"))
    exit(1)
  #print("Command succeeded: " + str(cmd) + ".\n Output:\n " + result.stdout.decode("utf-8"))

def build_and_install_nibi():
  print("Building and installing Nibi library and application")

  if not os.path.exists("./build"):
    os.mkdir("./build")

  os.chdir("./build")
  execute_command(["cmake", "..", "-DCMAKE_BUILD_TYPE=Release"])
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
  execute_command(["cmake", "..", "-DCMAKE_BUILD_TYPE=Release"])
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

  os.chdir(module_dir)

def build_and_install_modules():
  os.chdir("./modules")
  for module in os.listdir("."):
    print("Building and installing module: " + module)
    install_module(module)
    print("\n")

if not args.install_nibi and not args.install_modules:
  print("No arguments provided. Exiting.")
  exit(0)

if args.install_nibi:
  build_and_install_nibi()

if args.install_modules:
  if not os.path.exists("./modules"):
    print("No modules found. Exiting.")
    exit(0)
  print("\n")
  build_and_install_modules()
