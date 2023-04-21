#!/usr/bin/python3

import contextlib
import argparse

import multiprocessing

import sys
import os
import shutil
import shlex

import subprocess

@contextlib.contextmanager
def push_directory(new_dir: str, create_if_absent=True):
    if create_if_absent:
        os.makedirs(new_dir, exist_ok=True)

    previous_dir = os.getcwd()
    os.chdir(new_dir)
    try:
        yield
    finally:
        os.chdir(previous_dir)

def run_and_check(command: str, *, expect=0, error_msg="", stdout_quiet=False, stderr_quiet=False, quiet=False):
    stdout_redirect = None if not stdout_quiet and not quiet else subprocess.DEVNULL
    stderr_redirect = None if not stderr_quiet and not quiet else subprocess.DEVNULL
    print("CMD: {}".format(command))
    result = subprocess.run(shlex.split(command), stdout=stdout_redirect, stderr=stderr_redirect)
    if expect != None and result.returncode != expect:
        print("{} returned {} but expected {}".format(command, result.returncode, expect))
        if error_msg:
            print(error_msg)
        exit(result.returncode)

def current_execution_inside_docker():
    return os.path.exists("/.dockerenv")

def main(args: list):
    default_build_directory = os.path.join(os.path.dirname(os.path.abspath(__file__)), "artifacts")
    default_source_directory = os.path.abspath(__file__)

    parser = argparse.ArgumentParser(args)

    parser.add_argument("-c", "--clean", action="store_true", help="Clean before building")
    parser.add_argument("-d", "--directory", help="Directory to perform CMake build", default=default_build_directory)

    parsed_args = vars(parser.parse_args())

    if current_execution_inside_docker():
        run_and_check("chown -R validator .")
        run_and_check("git config --global --add safe.directory /home/validator")
        run_and_check("git submodule update --init --recursive")
        with push_directory(parsed_args["directory"]):
            run_and_check("cmake .. -DCMAKE_BUILD_TYPE=Debug")
            run_and_check("cmake --build . --parallel {}".format(multiprocessing.cpu_count()))
            run_and_check("{} --duration".format(os.path.join(parsed_args["directory"], "unittests")))
    else:
        run_and_check("docker --version", error_msg="Docker CLI missing. Please install Docker CLI first.", quiet=True)
        run_and_check("docker build --file Dockerfile --tag fedora:cpp-image .")
        run_and_check("docker rm -f cpp-container", expect=None, stderr_quiet=True)
        run_and_check("docker run --interactive --name cpp-container --detach fedora:cpp-image")
        run_and_check("docker exec --interactive --tty cpp-container /home/validator/build.py")

if __name__ == "__main__":
    main(sys.argv)

