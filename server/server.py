from fastapi import FastAPI, File, UploadFile, HTTPException
import requests
import tempfile
import os
import uuid
import shutil
# from google.cloud import speech
from dotenv import load_dotenv
from module import transcribe_audio, generate_unreal_code, JSONparser
from langchain_core.prompts import PromptTemplate
from langchain_google_genai import ChatGoogleGenerativeAI
from langchain_core.output_parsers import PydanticOutputParser
from module2 import *
import json


load_dotenv()
GEMINI_API_KEY = os.getenv("GEMINI_API_KEY")
llm =llm = ChatGoogleGenerativeAI(model='gemini-1.5-pro')

load_json_data()

app = FastAPI()

@app.post("/convert-audio")
async def convert_audio(file: UploadFile = File(...)):
    try:
        with tempfile.NamedTemporaryFile(delete=False, suffix=".wav") as tmp:
            tmp.write(await file.read())
            tmp_path = tmp.name
        text = transcribe_audio(tmp_path)
        os.remove(tmp_path)
        prompt = PromptTemplate.from_template(
            "출력은 항상 아래에 제시해주는 출력의 JSON형태로 출력해주세요.\n"
            "아래에 제시되는 명령어로부터 의미를 파악하여 아래의 출력의 형태에 맞게 채워주세요 \n"
            "도로롱 또는 도로롱과 비슷한 단어는 Lamball로 처리해주세요 \n"
            "'모두','전부'와 같은 단어가 포함되어 있다면 actor=Everyone으로 고정해주세요\n"
            "'빨리', '어서'와 같이 긴급함을 요하는 단어가 포함되면 forced=True로 해주세요. 없으면 False로 처리해주세요. \n"
            "target은 ['Stone','Tree','Ore']에서 골라주세요."
            "명령어: {command}\n"
            "출력:{format}"
            )
        parser = PydanticOutputParser(pydantic_object = JSONparser)
        prompt = prompt.partial(format=parser.get_format_instructions())
        chain = prompt | llm | parser
        output=chain.invoke(text)
        print(output)
        return output
    except:
        return {'error':"ERROR! PLEASE PRONOUNCE RIGHTLY"}



@app.post("/generate_Json")
async def convert_audio(file: UploadFile = File(...)):
    try:
        with tempfile.NamedTemporaryFile(delete=False, suffix=".wav") as tmp:
            tmp.write(await file.read())
            tmp_path = tmp.name
        text = transcribe_audio(tmp_path)
        os.remove(tmp_path)
        initialize_model()
        pals_data, items_data, tasks_data = load_json_data()
        pals_names = {pal["name"]: pal["name"] for pal in pals_data}
        tasks_names = {task["name"]: task["name"] for task in tasks_data}
        items_names = {item["name"]: item["name"] for item in items_data}
        llm_chain = setup_llm_chain(pals_names, tasks_names, items_names)
        user_command=text
        print(text)
        result = extract_command(user_command, llm_chain, pals_names, tasks_names, items_names)
        output = json.dumps(result, indent=4, ensure_ascii=False)
        if type(output)==str:
            output=json.loads(output)
        return output
    except:
        return {'error':"ERROR! PLEASE PRONOUNCE RIGHTLY"}
    







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

