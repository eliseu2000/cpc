from django.shortcuts import render, get_object_or_404
from django.http import JsonResponse, HttpResponse
from .models import Controlador


def view1(request):
    return HttpResponse("Bem vinda, Sthefany!")

def atualizacao_dados(request, numero_serial, entrada_analogica, entrada_digital):
    try:
        # Tenta localizar o objeto Controlador pelo numero_serial
        controlador = Controlador.objects.get(numero_serial=numero_serial)

        # Atualiza os campos entrada_analogica e entrada_digital
        controlador.entrada_analogica = entrada_analogica
        controlador.entrada_digital = entrada_digital

        # Salva o objeto atualizado no banco de dados
        controlador.save()

        # Retorna o objeto atualizado em formato JSON
        return JsonResponse({
            "numero_serial": controlador.numero_serial,
            "entrada_analogica": float(controlador.entrada_analogica),
            "saida_analogica": float(controlador.saida_analogica),
            "entrada_digital": controlador.entrada_digital,
            "saida_digital": controlador.saida_digital,
            "mensagem": "Dados atualizados!"
        })
    except Controlador.DoesNotExist:
        # Retorna uma mensagem de erro se o objeto não for encontrado
        return JsonResponse({"mensagem": "Controlador não encontrado!"}, status=404)

