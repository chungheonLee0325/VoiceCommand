from fastapi import FastAPI, File, UploadFile
import whisper
import requests
import os
import shutil

app = FastAPI()
# 위스퍼 모델
stt_model = whisper.load_model("base")
# 게임 서버 주소
GAME_SERVER_URL = "http://~~~~"

# 1. STT
@app.post("/stt")
async def stt_audio(file: UploadFile = File(...)):
    file_path = os.path.join("sound", file.filename)

    with open
    return("p")

@app.get("get1")
async def get1():
    return("async 넣은 get1입니다")

@app.get("get2")
async def get1():
    return("async 넣은 get2입니다")