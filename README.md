# child_mbed
こども端末</br>
mbedコンパイラのリンク先</br>
https://developer.mbed.org/users/kataokashunpei/code/01/</br>


##main.cpp
12行目　取得時間の変更　　


13行目　送信時間の変更 送信時間は設定時間からランダムに(0~10）秒を引く　　


14行目　IDを変更　例：0xAA = AA:AA:AA:AA:AA:AAとなる　　


全体のprintfのコメントアウトを外すことで、ターミナルに結果が出力されるようになります　　


##電波強度の変更箇所について


01/nRF51822.lib/TARGET_NRF5/source/projectconfig.h 100行目


この数値を変えることによって電波強度を変えることができる．


一番弱いのは-40


一番強いのは4


である.


今回の実験では-16を設定した．


原因はわかっていないがgithubにコードをアップするためmbedコンパイラからコード一式をダウンロードすると</br>
01/nRF51822.lib/TARGET_MCU_NRF51822</br>
のディレクトリがダウンロードできない．</br>
