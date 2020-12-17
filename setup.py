#!/usr/bin/env python
#-*- coding:utf-8 -*-

from setuptools import setup, find_packages
import sys
import json
import os
import shutil
import platform
import click


#------------------------------------------------------------------------------
# 前置检查
#------------------------------------------------------------------------------
def check_xmake():
    """检查是否按照了编译工具 xmake"""
    print("checking xmake ...")
    xmake = os.system("xmake --version")
    return False if xmake != 0 else True


def get_boost_envrionment():
    """ 
    获取 BOOST 环境变量设置
        @return (current_boost_root, current_boost_lib)
    """
    current_dir = os.getcwd()
    current_boost_root = ''
    current_boost_lib = ''
    if 'BOOST_ROOT' in os.environ:
        current_boost_root = os.environ['BOOST_ROOT']
        if 'BOOST_LIB' in os.environ:
            current_boost_lib = os.environ['BOOST_LIB']
        else:
            current_boost_lib = current_boost_root + '/stage/lib'
            os.environ['BOOST_LIB'] = current_boost_lib
    else:
        for dir in os.listdir():
            if len(dir) >= 5 and dir[:5] == 'boost' and os.path.isdir(dir):
                current_boost_root = current_dir + '/' + dir
                current_boost_lib = current_dir + '/' + dir + '/stage/lib'
                os.environ['BOOST_ROOT'] = current_boost_root
                os.environ['BOOST_LIB'] = current_boost_lib
    return (current_boost_root, current_boost_lib)


def get_python_version():
    """获取当前 python版本"""
    py_version = platform.python_version_tuple()
    py_version = int(py_version[0]) * 10 + int(py_version[1])
    print('current python version:', int(py_version) * 0.1)
    return py_version


def get_current_compile_info():
    """获取当前编译信息, 其中 mode 无效"""
    current_bits = 64 if sys.maxsize > 2**32 else 32
    if sys.platform == 'win32':
        current_arch = 'x64' if current_bits == 64 else 'x86'
    else:
        current_arch = 'x86_64' if current_bits == 64 else 'i386'

    py_version = get_python_version()
    current_boost_root, current_boost_lib = get_boost_envrionment()
    current_compile_info = {
        'plat': sys.platform,
        'arch': current_arch,
        'mode': '',
        'py_version': py_version,
        'boost_root': current_boost_root,
        'boost_lib': current_boost_lib
    }
    return current_compile_info


def get_history_compile_info():
    """获取历史编译信息"""
    try:
        with open('compile_info', 'r') as f:
            result = json.load(f)
    except:
        result = {
            'plat': '',
            'arch': '',
            'mode': '',
            'py_version': 0,
            'boost_root': '',
            'boost_lib': ''
        }
    return result


def save_current_compile_info(compile_info):
    """保持当前编译信息"""
    with open('compile_info', 'w') as f:
        json.dump(compile_info, f)


def build_boost(mode):
    """ 编译依赖的 boost 库 """
    current_boost_root, current_boost_lib = get_boost_envrionment()
    if current_boost_root == '' or current_boost_lib == '':
        print("Can't get boost environment!")
        return
    current_dir = os.getcwd()
    if sys.platform == 'win32':
        os.chdir(current_boost_root)
        if not os.path.exists('b2.exe'):
            os.system('bootstrap.bat')
        os.system(
            'b2 {} link=static runtime-link=shared address-model=64 -j 4 --with-date_time'
            ' --with-filesystem --with-system --with-test'.format(mode))
        os.system(
            'b2 {} link=shared runtime-link=shared address-model=64 -j 4 --with-python'
            ' --with-serialization'.format(mode))
        #os.system(
        #    'b2 {} link=shared runtime-link=shared address-model=64 -j 4 --with-python'
        #    ' --with-date_time --with-filesystem --with-system --with-test'
        #    ' --with-serialization'.format(mode))
        os.chdir(current_dir)
    else:
        cmd = 'cd {boost} ; if [ ! -f "b2" ]; then ./bootstrap.sh ; fi; '\
              './b2 {mode} link=shared address-model=64 -j 4 --with-python --with-serialization; '\
              './b2 {mode} link=static address-model=64 cxxflags=-fPIC -j 4 --with-date_time '\
              '--with-filesystem --with-system --with-test; '\
              'cd {current}'.format(boost=current_boost_root, mode=mode, current=current_dir)
        # cmd = 'cd {boost} ; if [ ! -f "b2" ]; then ./bootstrap.sh ; fi; '\
        #       './b2 {mode} link=shared address-model=64 -j 4 --with-python --with-serialization '\
        #       '--with-date_time --with-filesystem --with-system --with-test; '\
        #       'cd {current}'.format(boost=current_boost_root, mode=mode, current=current_dir)
        os.system(cmd)


def clear_with_python_changed(mode):
    """
    python版本发生变化时，清理之前的python编译结果
    应该仅在 pyhon 版本发生变化时被调用
    """
    current_plat = sys.platform
    current_bits = 64 if sys.maxsize > 2**32 else 32
    if current_plat == 'win32' and current_bits == 64:
        build_pywrap_dir = 'build\\{mode}\\windows\\x64\\.objs\\windows\\x64\\{mode}\\hikyuu_pywrap'.format(
            mode=mode)
    elif current_plat == 'win32' and current_bits == 32:
        build_pywrap_dir = 'build\\{mode}\\windows\\x86\\.objs\\windows\\x86\\{mode}\\hikyuu_pywrap'.format(
            mode=mode)
    elif current_plat == 'linux' and current_bits == 64:
        build_pywrap_dir = 'build/{mode}/linux/x86_64/.objs/linux/x86_64/{mode}/hikyuu_pywrap'.format(
            mode=mode)
    elif current_plat == 'linux' and current_bits == 32:
        build_pywrap_dir = 'build/{mode}/linux/i386/.objs/linux/i386/{mode}/hikyuu_pywrap'.format(
            mode=mode)
    elif current_plat == "darwin" and current_bits == 64:
        build_pywrap_dir = 'build/{mode}/macosx/x86_64/.objs/macosx/x86_64/{mode}/hikyuu_pywrap'.format(
            mode=mode)
    elif current_plat == "darwin" and current_bits == 32:
        build_pywrap_dir = 'build/{mode}/macosx/i386/.objs/macosx/i386/{mode}/hikyuu_pywrap'.format(
            mode=mode)
    else:
        print("************不支持的平台**************")
        exit(0)
    if os.path.lexists(build_pywrap_dir):
        shutil.rmtree(build_pywrap_dir)
    current_boost_root, _ = get_boost_envrionment()
    if os.path.lexists('{}/bin.v2/libs/python'.format(current_boost_root)):
        shutil.rmtree('{}/bin.v2/libs/python'.format(current_boost_root))


#------------------------------------------------------------------------------
# 执行构建
#------------------------------------------------------------------------------
def start_build(verbose=False, mode='release'):
    """ 执行编译 """
    global g_verbose
    g_verbose = verbose
    if not check_xmake():
        print("Please install xmake")
        return

    current_compile_info = get_current_compile_info()
    current_compile_info['mode'] = mode

    py_version = current_compile_info['py_version']
    if py_version != 0 and py_version < 31:
        print("Python version must >= 3.1 !")
        return

    current_boost_root = current_compile_info['boost_root']
    current_boost_lib = current_compile_info['boost_lib']
    if current_boost_root == '' or current_boost_lib == '':
        print("Please configure BOOST")
        exit(0)
    print('BOOST_ROOT:', current_boost_root)
    print('BOOST_LIB:', current_boost_lib)

    #如果 python版本或者编译模式发生变化，则编译依赖的 boost 库（boost.python)
    history_compile_info = get_history_compile_info()
    if py_version != history_compile_info[
            'py_version'] or history_compile_info['mode'] != mode:
        clear_with_python_changed(mode)
        print('\ncompile boost ...')
        build_boost(mode)
        os.system("xmake f {} -c -y -m {}".format("-v -D" if verbose else "",
                                                  mode))
    else:
        os.system("xmake f {} -y -m {}".format("-v -D" if verbose else "",
                                               mode))

    os.system("xmake -b {} hikyuu".format("-v -D" if verbose else ""))
    if mode == "release":
        os.system("xmake -b {} core".format("-v -D" if verbose else ""))

    # 保存当前的编译信息
    save_current_compile_info(current_compile_info)


#------------------------------------------------------------------------------
# 控制台命令
#------------------------------------------------------------------------------


@click.group()
def cli():
    pass


@click.command()
@click.option('-v', '--verbose', is_flag=True, help='显示详细的编译信息')
@click.option('-m',
              '--mode',
              default='release',
              type=click.Choice(['release', 'debug']),
              help='编译模式')
def build(verbose, mode):
    """ 执行编译 """
    start_build(verbose, mode)


@click.command()
@click.option('-all', "--all", is_flag=True, help="执行全部测试, 否则仅仅进行最小范围测试）")
@click.option("-compile", "--compile", is_flag=True, help='强制重新编译')
@click.option('-v', '--verbose', is_flag=True, help='显示详细的编译信息')
@click.option('-m',
              '--mode',
              default='release',
              type=click.Choice(['release', 'debug']),
              help='编译模式')
@click.option('-case', '--case', default='', help="执行指定的 TestCase")
def test(all, compile, verbose, mode, case):
    """ 执行单元测试 """
    current_compile_info = get_current_compile_info()
    current_compile_info['mode'] = mode
    history_compile_info = get_history_compile_info()
    if compile or current_compile_info != history_compile_info:
        start_build(verbose, mode)
    if all:
        os.system("xmake f --test=all --mode={}".format(mode))
        os.system("xmake -b {} unit-test".format("-v -D" if verbose else ""))
        os.system("xmake r unit-test {}".format('' if case ==
                                                '' else '-tc {}'.format(case)))
    else:
        os.system("xmake f --test=small --mode={}".format(mode))
        os.system("xmake -b {} small-test".format("-v -D" if verbose else ""))
        os.system("xmake r small-test {}".format(
            '' if case == '' else '-tc {}'.format(case)))


@click.command()
@click.option("-with_boost", "--with_boost", is_flag=True, help='清除相应的BOOST库')
def clear(with_boost):
    """ 清除当前编译设置及结果 """
    if os.path.lexists('.xmake'):
        print('delete .xmake')
        shutil.rmtree('.xmake')
    if os.path.lexists('build'):
        print('delete build')
        shutil.rmtree('build')
    if os.path.lexists('Hikyuu.egg-info'):
        print('delete Hikyuu.egg-info')
        shutil.rmtree('Hikyuu.egg-info')
    if os.path.exists('compile_info'):
        print('delete compile_info')
        os.remove('compile_info')
    for r, _, f_list in os.walk('hikyuu'):
        for name in f_list:
            if (name != 'UnRAR.exe' and len(name) > 4 and name[-4:] in ('.dll','.exe','.pyd')) \
                   or (len(name) > 8 and name[:9] == 'libboost_')  \
                   or (len(name) > 6 and name[-6:] == '.dylib'):
                print('delete', r + '/' + name)
                os.remove(os.path.join(r, name))
    print('clear finished!')
    os.system("xmake clean")


@click.command()
def uninstall():
    """ 卸载已安装的 python 包 """
    if sys.platform == 'win32':
        site_lib_dir = sys.base_prefix + "/lib/site-packages"
    else:
        usr_dir = os.path.expanduser('~')
        py_version = get_python_version()
        site_lib_dir = '{}/.local/lib/python{:>.1f}/site-packages'.format(
            usr_dir, py_version * 0.1)
    for dir in os.listdir(site_lib_dir):
        if dir == 'hikyuu' or (len(dir) > 6 and dir[:6] == 'Hikyuu'):
            print('delete', site_lib_dir + '/' + dir)
            shutil.rmtree(site_lib_dir + '/' + dir)
    print("Uninstall finished!")


@click.command()
def install():
    """ 编译并安装 Hikyuu python 库 """
    start_build(False, 'release')
    if sys.platform == 'win32':
        install_dir = sys.base_prefix + "\\Lib\\site-packages\\hikyuu"
    else:
        usr_dir = os.path.expanduser('~')
        install_dir = '{}/.local/lib/python{:>.1f}/site-packages/hikyuu'.format(
            usr_dir,
            get_python_version() * 0.1)
        try:
            shutil.rmtree(install_dir)
        except:
            pass
        os.makedirs(install_dir)
    os.system('xmake install -o "{}"'.format(install_dir))


@click.command()
def wheel():
    """ 生成 python 的 wheel 安装包 """
    # 尝试编译
    start_build(False, 'release')

    # 清理之前遗留的打包产物
    print("Clean up the before papackaging outputs ...")
    py_version = get_python_version()
    if os.path.lexists('Hikyuu.egg-info'):
        shutil.rmtree('Hikyuu.egg-info')
    if os.path.lexists('build/lib'):
        shutil.rmtree('build/lib')
    if os.path.lexists('build'):
        for bdist in os.listdir('build'):
            if len(bdist) >= 5 and bdist[:5] == 'bdist' and os.path.lexists(
                    bdist):
                shutil.rmtree(bdist)
    for x in os.listdir('hikyuu'):
        if x[:12] == 'boost_python':
            if x[12:14] != str(py_version):
                os.remove('hikyuu/{}'.format(x))

    # 构建打包命令
    print("start pacakaging bdist_wheel ...")
    current_plat = sys.platform
    current_bits = 64 if sys.maxsize > 2**32 else 32
    if current_plat == 'win32' and current_bits == 64:
        plat = "win-amd64"
    elif current_plat == 'win32' and current_bits == 32:
        plat = "win32"
    elif current_plat == 'linux' and current_bits == 64:
        plat = "manylinux1_x86_64"
    elif current_plat == 'linux' and current_bits == 32:
        plat = "manylinux1_i386"
    elif current_plat == 'darwin' and current_bits == 32:
        plat = "macosx_i686"
    elif current_plat == 'darwin' and current_bits == 64:
        plat = "macosx_10_9_x86_64"
    else:
        print("*********尚未实现该平台的支持*******")
        return
    if current_plat == 'win32':
        cmd = 'python sub_setup.py bdist_wheel --python-tag cp{} -p {}'.format(
            py_version, plat)
        print(cmd)
        os.system(cmd)
    else:
        cmd = 'python3 sub_setup.py bdist_wheel --python-tag cp{} -p {}'.format(
            py_version, plat)
        print(cmd)
        os.system(cmd)


@click.command()
def upload():
    """ 发布上传至 pypi，仅供发布者使用！！！ """
    if not os.path.lexists('dist'):
        print("Not found wheel package! Pleae wheel first")
        return
    print("current wheel:")
    for bdist in os.listdir('dist'):
        print(bdist)
    print("")
    val = input('Are you sure upload now (y/n)? (deault: n) ')
    if val == 'y':
        os.system("twine upload dist/*")


#------------------------------------------------------------------------------
# 添加 click 命令
#------------------------------------------------------------------------------
cli.add_command(build)
cli.add_command(test)
cli.add_command(clear)
cli.add_command(install)
cli.add_command(uninstall)
cli.add_command(wheel)
cli.add_command(upload)

if __name__ == "__main__":
    cli()
