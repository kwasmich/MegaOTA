FROM archlinux

RUN pacman -Syu --noconfirm --needed make avr-gcc avr-libc avrdude
# RUN pacman -Syu --noconfirm --needed git fakeroot binutils cmake ghostscript doxygen graphviz psutils gcc groff

# RUN useradd --no-create-home --shell=/bin/false build && usermod -L build
# USER build
# WORKDIR /tmp
# RUN git clone https://aur.archlinux.org/srecord.git
# WORKDIR /tmp/srecord
# RUN makepkg -si
# USER root

# Set `DEVCONTAINER` environment variable to help with orientation
ENV DEVCONTAINER=true
