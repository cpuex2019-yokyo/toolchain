from setuptools import setup, find_packages

setup(
    name='yokyo_toolchain',
    version="0.0.1",
    author='Takashi Yoneuchi',
    author_email='takahsi.yoneuchi@shift-js.info',
    packages = ['yokyo_toolchain'],
    install_requires = ['pyelftools'],
    entry_points={
        "console_scripts": [
            "yokyo_elf2bin=yokyo_toolchain.elf2bin:main",
            "yokyo_bin2coe=yokyo_toolchain.bin2coe:main",
            "yokyo_bin2flash=yokyo_toolchain.bin2flash:main"
        ]
    },
)
