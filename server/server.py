from fastapi import FastAPI, File, UploadFile, HTTPException
import requests
import tempfile
import os
import uuid
import shutil
from google.cloud import speech
from openai import OpenAI
from dotenv import load_dotenv
from module import transcribe_audio, generate_unreal_code

app = FastAPI()
# dotenv_path = "C:/Jeonghwan/Gamejam/VoiceCommand/ProjVoiceCommand/.env"
# load_dotenv()

# Google Cloud Speech-to-Text 설정
# client = speech.SpeechClient()
# GEMINI_API_KEY = os.getenv("GEMINI_API_KEY")

# 저장할 디렉토리 설정
UPLOAD_DIR = "uploads"
os.makedirs(UPLOAD_DIR, exist_ok=True)

@app.post("/upload-audio/")
async def upload_audio(file: UploadFile = File(...)):
    file_path = os.path.join(UPLOAD_DIR, file.filename)

    # 업로드된 파일을 서버에 저장
    with open(file_path, "wb") as buffer:
        shutil.copyfileobj(file.file, buffer)

    return {"filename": file.filename, "file_path": file_path}



# @app.post("/upload-audio", status_code=status.HTTP_202_ACCEPTED)
# async def upload_audio(
#     background_tasks: BackgroundTasks,
#     file: UploadFile = File(...),
#     job_store: Dict[str, JobResult] = Depends(get_job_store)
# ):
#     # 새 작업 ID 생성
#     job_id = str(uuid.uuid4())
    
#     try:
#         # 임시 파일 생성
#         with tempfile.NamedTemporaryFile(delete=False, suffix=".wav") as tmp:
#             tmp.write(await file.read())
#             tmp_path = tmp.name
        
#         # 작업 상태 초기화
#         job_store[job_id] = JobResult(status=JobStatus.PENDING)
        
#         # 백그라운드 작업으로 처리 시작
#         background_tasks.add_task(process_audio_file, job_id, tmp_path, job_store)
#         return {"job_id": job_id, "message": "Audio upload successful. Processing started."}
    
#     except Exception as e:
#         if job_id in job_store:
#             job_store[job_id].status = JobStatus.FAILED
#             job_store[job_id].error = str(e)
#         raise HTTPException(status_code=500, detail=str(e))

# @app.get("/job-status/{job_id}")
# def get_job_status(
#     job_id: str,
#     job_store: Dict[str, JobResult] = Depends(get_job_store)
# ):
#     if job_id not in job_store:
#         raise HTTPException(status_code=404, detail="Job not found")
#     job = job_store[job_id]
    
#     # 작업 완료 또는 실패 상태일 때 전체 결과 반환
#     if job.status in [JobStatus.COMPLETED, JobStatus.FAILED]:
#         return job
#     # 진행 중인 상태일 때는 상태만 반환
#     return {"status": job.status}

# @app.get("/get-code/{job_id}")
# def get_code(
#     job_id: str,
#     job_store: Dict[str, JobResult] = Depends(get_job_store)
# ):
#     if job_id not in job_store:
#         raise HTTPException(status_code=404, detail="Job not found")
    
#     job = job_store[job_id]
    
#     if job.status == JobStatus.FAILED:
#         raise HTTPException(status_code=500, detail=job.error)
#     if job.status != JobStatus.COMPLETED:
#         raise HTTPException(status_code=400, detail=f"Job is still in progress. Current status: {job.status}")
#     return {"transcribed_text": job.transcribed_text, "unreal_code": job.unreal_code}

# # 작업 정리를 위한 엔드포인트 (선택 사항)
# @app.delete("/job/{job_id}")
# def delete_job(
#     job_id: str,
#     job_store: Dict[str, JobResult] = Depends(get_job_store)
# ):
#     if job_id not in job_store:
#         raise HTTPException(status_code=404, detail="Job not found")
    
#     del job_store[job_id]
#     return {"message": "Job deleted successfully"}

