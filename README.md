# m5-temp

m5stackで作った温度計

## feature

M5でwifiに接続し温度をjson形式でサーバに送ります。
サーバーはajaxで温度情報を1秒に一回取得しchart.jsでグラフに変換します。

## 表示サンプル

実機が無くても見た目を確認できるよう、[GitHub Pages](https://5ym.github.io/m5-temp/) では記録済みデータをループ再生します。
(ホスト名が `github.io` の場合、または URL に `?demo` を付けた場合にサンプル再生モードになります)
