
#include <iostream>

#include "playback.h"
using namespace std;

int main() {
  shared_ptr<Fourier::Aios> aios = make_shared<Fourier::Aios>();
  Fourier::Playback playback(aios);
  playback.setFrequency(100);

  int sel = 0;
  cout << "******* 示教再现采集系统 *******" << endl;
  while (true) {
    cout << "1.开始单点采集" << endl;
    cout << "2.开始实时采集" << endl;
    cout << "3.添加采集点" << endl;
    cout << "4.清空采集点" << endl;
    cout << "5.示教再现" << endl;
    cout << "6.同步模式" << endl;
    cout << "7.退出系统" << endl;
    cin >> sel;
    switch (sel) {
      case 1:
        cout << "******* 开始单点采集，点击回车记录当前位置 *******" << endl;
        playback.gatherPos();
        break;
      case 2:
        cout << "******* 开始实时采集 *******";
        playback.gatherPos(true);
        break;
      case 3:
        cout << "******* 添加采集点 *******";
        playback.gatherPos(true, ios::app);
        break;
      case 4:
        cout << "******* 清空采集点 *******" << endl;
        playback.clearDate();
        break;
      case 5:
        cout << "******* 开始示教再现 *******" << endl;
        playback.playback();
        break;
      case 6:
        cout << "******* 开始示教再现 *******" << endl;
        playback.synchronousMode();
        break;
      case 7:
        cout << "******* 退出系统 *******" << endl;
        return 0;
        break;
      default:
        break;
    }
  }

  return 0;
}
