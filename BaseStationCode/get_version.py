# https://community.platformio.org/t/is-it-possible-to-use-an-ifdef-to-distinquish-between-platform-versions/20776/7
Import("env")
from platformio.builder.tools.piolib import PackageItem
from platformio.package.version import get_original_version

print("Adding platform and package version defines")

platform = env.PioPlatform()
used_packages = platform.dump_used_packages()
pkg_metadata = PackageItem(platform.get_dir()).metadata
platform_version = str(pkg_metadata.version if pkg_metadata else platform.version)
platform_version_split = platform_version.split(sep=".")

env.Append(CPPDEFINES=[
#   ("PIO_PLATFORM_VERSION_MAJOR", platform_version[0]),
#   ("PIO_PLATFORM_VERSION_MINOR", platform_version[1]),
#   ("PIO_PLATFORM_VERSION_RELEASE", platform_version[2]),
  ("PIO_PLATFORM_VERSION", f'\\"{platform_version}\\"')
])

for package in used_packages:
    pio_package_version = package["version"] # e.g. "1.70300.191015"
    pio_package_name = package["name"] # e.g. "toolchain-atmelavr"