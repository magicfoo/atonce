


@set ANDSDK_PATH=%ATBASE%\sugar\subuild\android_ndk\sdk_utils
@set JAVA_PROJ=C:\work\prog\sugar\proj\android
@set SRC_FILES=src\*.java gen\com\atonce\sugar\R.java
@set APK_NAME=SuActivity-debug
@set ASSET_PATH=%ATBASE%\sugar\suapp\fwtest\data
@rem %JAVA_PROJ%\assets

@mkdir %JAVA_PROJ%\gen
@mkdir %JAVA_PROJ%\bin
@mkdir %JAVA_PROJ%\bin\classes


@echo Generate R.java (aapt)
@%ANDSDK_PATH%\platform-tools\aapt package -f -m -M %JAVA_PROJ%\AndroidManifest.xml -S %JAVA_PROJ%\res -I %ANDSDK_PATH%\platforms\android-8\android.jar -J %JAVA_PROJ%\gen


@echo Compile java source file (javac)
@javac -d %JAVA_PROJ%\bin\classes -classpath "%JAVA_PROJ%\bin\classes;%JAVA_PROJ%;%JAVA_HOME%\lib\tools.jar" -sourcepath %JAVA_PROJ%\src;%JAVA_PROJ%\gen -target 1.5 -bootclasspath %ANDSDK_PATH%\platforms\android-8\android.jar;%ANDSDK_PATH%\add-ons\addon_google_apis_google_inc_8\libs\maps.jar -encoding ascii -g -source 1.5 %SRC_FILES%


@echo Generate dex file (dx.bat)
@call %ANDSDK_PATH%\platform-tools\dx.bat --dex --output=%JAVA_PROJ%\bin\classes.dex %JAVA_PROJ%\bin\classes


@echo Build tmp package (aapt)
@%ANDSDK_PATH%\platform-tools\aapt package -f --debug-mode -M %JAVA_PROJ%\AndroidManifest.xml -S %JAVA_PROJ%\res -A %ASSET_PATH% -I %ANDSDK_PATH%\platforms\android-8\android.jar -F %JAVA_PROJ%\bin\%APK_NAME%.ap_ -0 ""


@echo Build unsigned package (apkbuilder)
@call %ANDSDK_PATH%\tools\apkbuilder.bat %JAVA_PROJ%\bin\%APK_NAME%-unaligned.apk -z %JAVA_PROJ%\bin\%APK_NAME%.ap_ -nf %JAVA_PROJ%\libs -f %JAVA_PROJ%\bin\classes.dex 2>&0


@echo Build final package (zipalign)
@%ANDSDK_PATH%\tools\zipalign.exe -f 4 %JAVA_PROJ%\bin\%APK_NAME%-unaligned.apk %JAVA_PROJ%\bin\%APK_NAME%.apk 
