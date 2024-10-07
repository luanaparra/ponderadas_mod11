from transformers import pipeline

generator = pipeline('text-generation', model='distilgpt2')

prompt = "The Raspberry Pi is"
results = generator(prompt, max_length=50, num_return_sequences=1)

for i, result in enumerate(results):
    print(f"Resultado {i+1}: {result['generated_text']}")
