# heatmap
外部カメラから得た映像から映像内のR要素の差分を用いたヒートマップを作成するプログラムを開発しているリポジトリ.  
これを利用しRedの群誘導実験の領域網羅率を抽出する. 

## 使用環境
Mac OS環境での使用を推奨.

## 依存関係
- CMake  
- Opencv

## 使用方法
### リアルタイム使用
```
cd heatmap
cd build
cmake ..
./build/main
```
### 持っている動画を使用
```
cd heatmap
cd build
cmake ..
./build/main ${video_file_path}
```

### 注意点
プログラムが開始すると, カメラが起動するためヒートマップ化させたい領域をUIを使用し, ディスプレイ上で複数点左クリックすることで指定する.  
右クリックをすることで領域の選択が終了しヒートマップの作成と元動画の撮影が開始する.  
領域の点を選択する際, 領域は選ばれた点をなぞるように矩形を作成するため, 連続する点を選ぶことに注意. 

Opencvについてパスが通っていないとエラーが発生するため注意

main.cppのパラメータを変更することで, 差分の閾値やカメラの設定なども変更可能. 

## 変更課題
trackingは作成途中なので使用不可.


