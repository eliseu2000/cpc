from django.db import models

# Create your models here.
class Controlador(models.Model):
    numero_serial = models.CharField(max_length=20)
    entrada_analogica = models.DecimalField(max_digits=4, decimal_places=2)
    saida_analogica = models.DecimalField(max_digits=4, decimal_places=2)
    entrada_digital = models.IntegerField()
    saida_digital = models.IntegerField()

    def __str__(self):
        return self.numero_serial
