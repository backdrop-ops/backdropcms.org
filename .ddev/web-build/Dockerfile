ARG BASE_IMAGE
FROM $BASE_IMAGE

# Install Backdrop CLI tool bee:
RUN wget -qO bee.zip https://github.com/backdrop-contrib/bee/archive/1.x-1.x.zip
RUN unzip -q bee.zip && rm bee.zip
RUN mv bee-1.x-1.x /usr/local/bin/bee
