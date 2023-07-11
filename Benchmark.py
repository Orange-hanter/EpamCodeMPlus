import time
import os
from subprocess import Popen
from statistics import mean

wd_path = "/home/daniil/Git/EpamCodeMPlus"
app_path = f"{wd_path}/cmake-build-debug-gcc-1230-x86_64/Src/Clone.App/Clone.App"
copy_path = f"{wd_path}/copy"
filename = f"{wd_path}/file"

multithread_stat = [0.0000000001]
ipc_stat = [0.0000000001]
net_stat = [0.0000000001]


def statistic(result_list=None):
    def timer_func(func):
        def wrapper(*args, **kwargs):
            start = time.time()
            value = func(*args, **kwargs)
            end = time.time()
            runtime = end - start
            if isinstance(result_list, list):
                result_list.append(runtime)
            msg = "{func} took {time} seconds to complete its execution."
            print("\n", msg.format(func=func.__name__, time=runtime), "\n")
            # os.remove(copy_path)
            return value

        return wrapper

    return timer_func


def generate_file(size=100):
    with open(filename, 'wb') as f:
        kb = 1024
        mb = kb * 1024

        f.seek(size * mb)  # 100 Mb default
        f.write(b'0')


@statistic(multithread_stat)
def multithread_way(path, copy_path):
    os.system(app_path + f" -s {path} -d {copy_path}")


@statistic(ipc_stat)
def ipc_way(path, copy_path):
    Popen([app_path, "-s", filename, "--ipc"])
    os.system(app_path + f" -s {copy_path} --ipc --client")


@statistic(net_stat)
def net_way(path, copy_path):
    os.system(app_path + f" --client --source {path} --mode NET --destination 127.0.0.1")


if __name__ == "__main__":
    MB = 100
    generate_file(MB)
    net_test = Popen([app_path, "--source", copy_path, "--mode", "NET"])

    for _ in range(2):
        multithread_way(filename, copy_path)
        # ipc_way(filename, copy_path)
        net_way(filename, copy_path)
    net_test.kill()

    for cp_type, rate in [("Network", net_stat), ("Multithread", multithread_stat)]:
        # TODO make sure the measure value is correct
        print(f"Average {cp_type} rate: {MB / mean(rate) :.3f} MB/s")
    os.remove(filename)
