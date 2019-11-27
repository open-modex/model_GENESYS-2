import time
import datetime


def res_recognition():
    # todo: calculate resolution from time array, datetime object or similar "resolution"
    pass


def daily_values(resolution=6):
    values_per_day = int(24 / resolution)
    return values_per_day


def current_datetime():
    return datetime.datetime.now().strftime("%Y-%m-%d_%H_%M_%S")


def timer(func):
    def f(*args, **kwargs):
        before = time.clock()
        rv = func(*args, **kwargs)
        after = time.clock()
        print('elapsed time for {.__name__}: {:.2f} minutes'.format(func, (after - before)/60))
        return rv
    return f

