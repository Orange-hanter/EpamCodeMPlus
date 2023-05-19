import timeit
import time
import os
import subprocess
from statistics import mean 


wd_path = "C:/Users/Daniil_Akhramiuk/source/repos/Orange-hanter/EpamCodeMPlus"
app_path = f"{wd_path}/build/Clone/Clone.App/Debug/Clone.App.exe" 
copy_path = f"{wd_path}/copy"
filename = f"{wd_path}/file"

multithread_stat = []
ipc_stat = []

def statistic(result_list = None):
    def timer_func(func):
        def wrapper(*args, **kwargs):
            start = time.time()
            value = func(*args, **kwargs)
            end = time.time()
            runtime = end - start
            if isinstance(result_list, list): result_list.append(runtime)
            msg = "{func} took {time} seconds to complete its execution."
            print("\n", msg.format(func = func.__name__,time = runtime), "\n")
            os.remove(copy_path)
            return value
        return wrapper
    return timer_func


def generate_file(size = 100):
    with open(filename, 'wb') as f:
        kb = 1024
        mb = kb * 1024

        f.seek(size * mb) # 100 Mb default
        f.write(b'0')

@statistic(multithread_stat)
def multithread_way(path, copy_path):
    os.system(app_path + f" -s {path} -d {copy_path}")

@statistic(ipc_stat)
def ipc_vay(path, copy_path):
    subprocess.Popen([app_path,  "-s", filename, "--ipc"])
    os.system(app_path + f" -s {copy_path} --ipc --client")

if __name__ == "__main__":
    MB = 100
    generate_file(MB)
    
    for _ in range(2):
        multithread_way(filename, copy_path)
        ipc_vay(filename, copy_path)
    #TODO make sure the measure value is correct
    print(f"Average copying speed is: {mean(multithread_stat)/MB} MiB/s")
    print(f"Average ipc time: {mean(ipc_stat)/MB} MiB/s")
    os.remove(filename)
    