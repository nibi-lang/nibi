# Ubuntu Base
FROM ubuntu:22.04

# Installing essencial packages for python and C/C++
RUN apt-get update && apt-get install -y git g++-9 pkg-config build-essential cmake python3 libffi-dev sudo

# Workding DIR
WORKDIR /NIBI

# Copying the files
COPY . .
RUN mkdir /NIBI/build

# Set NIBI_PATH for executable
ENV NIBI_PATH=/NIBI/build

# Running commander.py with -n -m flags
RUN python3 commander.py -n -m

# Find the nibi shared lib
CMD ["sudo", "ldconfig"]

# Open nibi REPL
ENTRYPOINT ["nibi"]