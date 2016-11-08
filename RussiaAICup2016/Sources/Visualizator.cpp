#ifdef ENABLE_VISUALIZATOR

#include "Visualizator.h"
#if (defined _WIN32 || defined _WIN64)
# include <winsock2.h>
# include <Ws2tcpip.h>

#include <BaseTsd.h>
typedef SSIZE_T ssize_t;

ssize_t close(SOCKET s) {
  return closesocket(s);
}

ssize_t write(SOCKET s, const char *buf, int len, int flags = 0) {
  return send(s, buf, len, flags);
}

#pragma warning(disable: 4996)
#else
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#endif
#include <cstdio>
#include <cstdlib>

#include <string>

using namespace AICup;

std::string Visualizator::DEFAULT_HOST = "127.0.0.1";
std::string Visualizator::DEFAULT_PORT = "13579";

void Visualizator::setWindowCenter(double x, double y, double maxWidth, double maxHeight) {
  windowCenterX = floor(x / 800) * 800 + 400;
  windowCenterX = max(windowCenterX, windowWidth);
  windowCenterX = min(windowCenterX, maxWidth - windowWidth);

  windowCenterY = floor(y / 800) * 800 + 400;
  windowCenterY = max(windowCenterY, windowHeight);
  windowCenterY = min(windowCenterY, maxHeight - windowHeight);
}

void Visualizator::init() {
  bufMemory.resize(BUF_SIZE);
  buf = bufMemory.data();

  windowCenterX = 0;
  windowCenterY = 0;
  windowWidth = 800 * 4;
  windowHeight = 800 * 3;
}

Visualizator::Visualizator() : openSocket(INVALID_SOCKET) {
  /* Obtain address(es) matching host/port */
  addrinfo hints;
  memset(&hints, 0, sizeof(addrinfo));
  hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
  hints.ai_socktype = SOCK_STREAM; /* Datagram socket */
  hints.ai_flags = 0;
  hints.ai_protocol = 0;          /* Any protocol */

  addrinfo* result = NULL;
  INT success = getaddrinfo(DEFAULT_HOST.c_str(), DEFAULT_PORT.c_str(), &hints, &result);
  if (0 != success) {
    fprintf(stderr, "Could not get address");
    return;
  }


  for (addrinfo* rp = result; NULL != rp; rp = rp->ai_next) {
    SOCKET sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
    if (INVALID_SOCKET == sfd) {
      continue;
    }

    if (-1 == connect(sfd, rp->ai_addr, rp->ai_addrlen)) {
      close(sfd);
      continue;
    }

    openSocket = sfd;
    break;/* Success */
  }

  freeaddrinfo(result);

  if (INVALID_SOCKET == openSocket) {/* No address succeeded */
    fprintf(stderr, "Could not connect\n");
  }
}

void Visualizator::sendCommand(const char* str) const {
  if (INVALID_SOCKET == openSocket) {
    return;
  }

  int len = strlen(str);
  int pos = 0;
  while (pos < len) {
    ssize_t res = write(openSocket, str + pos, len);
    if (-1 == res) {
      fprintf(stderr, "Couldn't send command");
      return;
    }
    pos += res;
  }
}

void Visualizator::beginPre() const {
  sendCommand("begin pre\n");
}

void Visualizator::endPre() const {
  sendCommand("end pre\n");
}

void Visualizator::beginPost() const {
  sendCommand("begin post\n");
}

void Visualizator::endPost() const {
  sendCommand("end post\n");
}

void Visualizator::writeWithColor(char* buf, int32_t color) const {
  size_t len = strlen(buf);
  double r = ((color & 0xFF0000) >> 16) / 256.0;
  double g = ((color & 0x00FF00) >> 8) / 256.0;
  double b = ((color & 0x0000FF)) / 256.0;
  sprintf(buf + len, " %.3f %.3f %.3f\n", r, g, b);
  sendCommand(buf);
}

void Visualizator::circle(double x, double y, double r, int32_t color) const {
  if (INVALID_SOCKET != openSocket && checkPoint(x, y)) {
    sprintf(buf, "circle %.3f %.3f %.3f", x, y, r);
    writeWithColor(buf, color);
  }
}

void Visualizator::fillCircle(double x, double y, double r, int32_t color) const {
  if (INVALID_SOCKET != openSocket && checkPoint(x, y)) {
    sprintf(buf, "fill_circle %.3f %.3f %.3f", x, y, r);
    writeWithColor(buf, color);
  }
}

void Visualizator::rect(double x1, double y1, double x2, double y2, int32_t color) const {
  if (INVALID_SOCKET != openSocket && (checkPoint(x1, y1) || checkPoint(x2, y2))) {
    sprintf(buf, "rect %.3f %.3f %.3f %.3f", x1, y1, x2, y2);
    writeWithColor(buf, color);
  }
}

void Visualizator::fillRect(double x1, double y1, double x2, double y2, int32_t color) const {
  if (INVALID_SOCKET != openSocket && (checkPoint(x1, y1) || checkPoint(x2, y2))) {
    sprintf(buf, "fill_rect %.3f %.3f %.3f %.3f", x1, y1, x2, y2);
    writeWithColor(buf, color);
  }
}

void Visualizator::line(double x1, double y1, double x2, double y2, int32_t color) const {
  if (INVALID_SOCKET != openSocket && (checkPoint(x1, y1) || checkPoint(x2, y2))) {
    sprintf(buf, "line %.3f %.3f %.3f %.3f", x1, y1, x2, y2);
    writeWithColor(buf, color);
  }
}

void Visualizator::text(double x, double y, const char* text, int32_t color) const {
  if (INVALID_SOCKET != openSocket && checkPoint(x, y)) {
    sprintf(buf, "text %.3f %.3f %s", x, y, text);
    writeWithColor(buf, color);
  }
}

void Visualizator::text(double x, double y, double value, int32_t color) const {
  if (INVALID_SOCKET != openSocket && checkPoint(x, y)) {
    sprintf(buf, "text %.3f %.3f %.2f", x, y, value);
    writeWithColor(buf, color);
  }
}

void Visualizator::text(double x, double y, int64_t value, int32_t color) const {
  if (INVALID_SOCKET != openSocket && checkPoint(x, y)) {
    sprintf(buf, "text %.3f %.3f %I64d", x, y, value);
    writeWithColor(buf, color);
  }
}

bool Visualizator::checkPoint(double x, double y) const {
  return windowCenterX - windowWidth < x && x < windowCenterX + windowWidth &&
    windowCenterY - windowHeight < y && y < windowCenterY + windowHeight;
}

#endif