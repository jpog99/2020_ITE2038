SELECT DISTINCT name
FROM CatchedPokemon INNER JOIN Pokemon as p ON pid=p.id
WHERE owner_ID IN (SELECT id FROM Trainer WHERE hometown ='Sangnok City') AND
      pid IN (SELECT pid FROM CatchedPokemon WHERE owner_id IN(
              SELECT id FROM Trainer WHERE hometown ='Brown City'));