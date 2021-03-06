#include "IAudioPlay.h"
#include "XLog.h"

void IAudioPlay::Update(XData data) {
    //压入缓冲队列
    XLOGI("IAudioPlay::Update %d", data.size);

    if (data.size <= 0 || !data.data) return;

    while (!isExit) {
        framesMutex.lock();

        //队列长度超过极值，进入等待状态
        if (frames.size() > maxFrames) {
            XLOGI("IAudioPlay::Update audio list is full ,waiting...");
            framesMutex.unlock();
            XSleep(1);
            continue;
        }
        frames.push_back(data);
        framesMutex.unlock();
        break;
    }
}

//清理音频队列
void IAudioPlay::Clear(){
    framesMutex.lock();
    while(!frames.empty()){
        frames.front().Drop();
        frames.pop_front();
    }
    framesMutex.unlock();
}

XData IAudioPlay::GetData() {
    XData d;
    isRunning = true;
    while (!isExit) {

        //判断是否暂停
        if(IsPause()){
            XSleep(2);
            XLOGI("IAudioPlay::GetData pause");
            continue;
        }
        framesMutex.lock();
        if (!frames.empty()) {
            d = frames.front();
            frames.pop_front();
            framesMutex.unlock();
            pts = d.pts;
            return d;
        }
        framesMutex.unlock();
        XSleep(1);
    }
    isRunning = false;
    return d;
}