#pragma once

#include <core/SkPoint.h>
#include <core/SkRect.h>
#include <string>

namespace core {
    class Point {
        float x=0, y=0;
        bool isRatio = false;
        int windowWidth = 0;
        int windowHeight = 0;
    public:
        Point(float x, float y, bool isRatio = false) : x(x), y(y), isRatio(isRatio) {}
        Point() {}
        operator SkPoint() const { 
            if(isRatio) {
                return SkPoint::Make(x * windowWidth, y * windowHeight);
            } else {
                return SkPoint::Make(x, y);
            }
        }
        void resize(int w, int h) {
            windowWidth = w;
            windowHeight = h;
        }
        void setRatio(bool ratio) {
            isRatio = ratio;
        }
        void setPosition(float x, float y) {
            this->x = x;
            this->y = y;
        }
        float getOX() const { return x; }
        float getOY() const { return y; }
        float getX() const { return isRatio ? x * windowWidth : x; }
        float getY() const { return isRatio ? y * windowHeight : y; }
        std::string toString() const {
            return "Point(" + std::to_string(getX()) + ", " + std::to_string(getY()) + ")";
        }
    };
    class Rect {
    public:
        Point p1, p2;
        Rect(float x, float y, float width, float height, bool isRatio = false) : p1(x, y, isRatio), p2(x + width, y + height, isRatio) {}
        Rect(const Point& p1, const Point& p2) : p1(p1), p2(p2) {}
        Rect(){}

        bool contains(float px, float py) const {
            return px >= p1.getX() && px <= p2.getX() && py >= p1.getY() && py <= p2.getY();
        }
        void resize(int w, int h) {
            p1.resize(w, h);
            p2.resize(w, h);
        }
        void setRatio(bool ratio) {
            p1.setRatio(ratio);
            p2.setRatio(ratio);
        }
        void setPosition(float x, float y, float width, float height) {
            p1.setPosition(x, y);
            p2.setPosition(x + width, y + height);
        }
        void setWH(float width, float height) {
            p2.setPosition(p1.getOX() + width, p1.getOY() + height);
        }
        float getX() const { return p1.getX(); }
        float getY() const { return p1.getY(); }
        float getEX() const { return p2.getX(); }
        float getEY() const { return p2.getY(); }
        float getWidth() const { return p2.getX() - p1.getX(); }
        float getHeight() const { return p2.getY() - p1.getY();}
        operator SkRect() const {
            return SkRect::MakeLTRB(p1.getX(), p1.getY(), p2.getX(), p2.getY());
        }
        std::string toString() const {
            return "Rect(" + std::to_string(getX()) + ", " + std::to_string(getY()) + ", " + std::to_string(getEX()) + ", " + std::to_string(getEY()) + ")";
        }
    };
    class SubRect: public Rect {
    public:
        Point p1, p2;
        Rect* parent;
        bool isRatio = false;
        int windowWidth = 0;
        int windowHeight = 0;
        SubRect(Rect* parent, float x, float y, float width, float height, bool isRatio = false) : parent(parent), p1(x, y, isRatio), p2(x + width, y + height, isRatio), isRatio(isRatio) {}
        bool contains(float px, float py) const {
            return px >= p1.getX() && px <= p2.getX() && py >= p1.getY() && py <= p2.getY() && parent->contains(px, py);
        }
        void resize(int w, int h) {
            p1.resize(w, h);
            p2.resize(w, h);
            windowWidth = w;
            windowHeight = h;
        }
        void setRatio(bool ratio) {
            p1.setRatio(ratio);
            p2.setRatio(ratio);
            isRatio = ratio;
        }
        void setPosition(float x, float y, float width, float height) {
            p1.setPosition(x, y);
            p2.setPosition(x + width, y + height);
        }
        void setWH(float width, float height) {
            p2.setPosition(p1.getOX() + width, p1.getOY() + height);
        }
        float getX() const { 
            if(!parent) return 0;
            if(isRatio) {
                return parent->getWidth() * p1.getOX() + parent->getX();
            }
            return parent->getX() + p1.getOX();
        }
        float getY() const { 
            if(!parent) return 0;
            if(isRatio) {
                return parent->getHeight() * p1.getOY() + parent->getY();
            }
            return parent->getY() + p1.getOY();
        }
        float getEX() const { 
            if(!parent) return 0;
            if(isRatio) {
                return parent->getWidth() * (p2.getOX()-p1.getOX()) + parent->getX();
            }
            return parent->getX() + p2.getOX();
        }
        float getEY() const { 
            if(!parent) return 0;
            if(isRatio) {
                return parent->getHeight()*(p2.getOY()-p1.getOY())+parent->getY();
            }
            return parent->getY() + p2.getOY();
        }
        float getWidth() const { return getEX() - getX(); }
        float getHeight() const { return getEY() - getY();}
        operator SkRect() const {
            return SkRect::MakeLTRB(getX(), getY(), getEX(), getEY());
        }
        std::string toString() const {
            return "SubRect(" + std::to_string(getX()) + ", " + std::to_string(getY()) + ", " + std::to_string(getEX()) + ", " + std::to_string(getEY()) + ")";
        }
    };
}