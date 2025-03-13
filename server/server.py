from fastapi import FastAPI, File, UploadFile, HTTPException
import requests
import tempfile
import os
from google.cloud import speech
from openai import OpenAI
from dotenv import load_dotenv
from module import transcribe_audio, generate_unreal_code

app = FastAPI()

# Google Cloud Speech-to-Text 설정
client = speech.SpeechClient()
GEMINI_API_KEY = os.getenv("GEMINI_API_KEY")

@app.get("/")
def home():
    return {"message": "안녕하살법! 하지메 마시떼!"}


# @app.post("/convert-audio")
# async def convert_audio(file: UploadFile = File(...)):
#     try:
#         with tempfile.NamedTemporaryFile(delete=False, suffix=".wav") as tmp:
#             tmp.write(await file.read())
#             tmp_path = tmp.name
        
#         text = transcribe_audio(tmp_path)
#         os.remove(tmp_path)
        
#         if not text:
#             raise HTTPException(status_code=400, detail="Failed to transcribe audio")
        
#         unreal_code = generate_unreal_code(text)
#         return {"transcribed_text": text, "unreal_code": unreal_code}
#     except Exception as e:
#         raise HTTPException(status_code=500, detail=str(e))
